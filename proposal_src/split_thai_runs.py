import sys, os, re, copy, tempfile, zipfile
import xml.etree.ElementTree as ET

W = 'http://schemas.openxmlformats.org/wordprocessingml/2006/main'
XML = 'http://www.w3.org/XML/1998/namespace'
ET.register_namespace('w', W)
qn = lambda t: f'{{{W}}}{t}'
THAI = re.compile(r'[฀-๿]')
LATIN = re.compile(r'[A-Za-zÀ-ɏ]')
NSFIX = {'w14': 'http://schemas.microsoft.com/office/word/2010/wordml',
         'w15': 'http://schemas.microsoft.com/office/word/2012/wordml',
         'wp14': 'http://schemas.microsoft.com/office/word/2010/wordprocessingDrawing'}

def classify(ch):
    if THAI.match(ch): return 'thai'
    if LATIN.match(ch): return 'latin'
    return None

def segment(text):
    segs, cur, buf = [], None, ''
    for ch in text:
        s = classify(ch)
        if s is None or s == cur:
            buf += ch
        elif cur is None and not segs:
            buf += ch; cur = s
        else:
            if buf: segs.append((cur, buf))
            cur, buf = s, ch
    if buf: segs.append((cur, buf))
    return segs

def ensure_cs(rpr):
    has = lambda t: rpr.find(qn(t)) is not None
    if not has('cs'):
        tail = [qn('em'), qn('lang'), qn('eastAsianLayout'), qn('specVanish')]
        idx = next((i for i, c in enumerate(rpr) if c.tag in tail), len(list(rpr)))
        rpr.insert(idx, ET.Element(qn('cs')))
    for a, b in (('b', 'bCs'), ('i', 'iCs')):
        if has(a) and not has(b):
            el = rpr.find(qn(a))
            nb = ET.Element(qn(b))
            if el.get(qn('val')) is not None:
                nb.set(qn('val'), el.get(qn('val')))
            rpr.insert(list(rpr).index(el) + 1, nb)
    sz = rpr.find(qn('sz'))
    if sz is not None and not has('szCs'):
        szcs = ET.Element(qn('szCs')); szcs.set(qn('val'), sz.get(qn('val')))
        rpr.insert(list(rpr).index(sz) + 1, szcs)

def process(path):
    tree = ET.parse(path); root = tree.getroot()
    pmap = {c: p for p in root.iter() for c in p}
    for r in list(root.iter(qn('r'))):
        ts = r.findall(qn('t'))
        if len(ts) != 1: continue
        text = ts[0].text or ''
        if not THAI.search(text): continue
        segs = segment(text)
        if len(segs) <= 1 or all(s in ('thai', None) for s, _ in segs):
            rpr = r.find(qn('rPr'))
            if rpr is None:
                rpr = ET.Element(qn('rPr')); r.insert(0, rpr)
            ensure_cs(rpr); continue
        parent = pmap.get(r)
        if parent is None: continue
        idx = list(parent).index(r); parent.remove(r)
        for k, (script, chunk) in enumerate(segs):
            nr = copy.deepcopy(r)
            nt = nr.find(qn('t')); nt.text = chunk
            nt.set(f'{{{XML}}}space', 'preserve')
            if script == 'thai':
                rpr = nr.find(qn('rPr'))
                if rpr is None:
                    rpr = ET.Element(qn('rPr')); nr.insert(0, rpr)
                ensure_cs(rpr)
            parent.insert(idx + k, nr)
    tree.write(path, xml_declaration=True, encoding='UTF-8')
    xml = open(path, encoding='utf-8').read()
    m = re.search(r'<w:(document|hdr|ftr)\b[^>]*>', xml)
    if m:
        tag = new = m.group(0)
        for pfx, uri in NSFIX.items():
            if f'xmlns:{pfx}=' not in tag:
                new = new[:-1] + f' xmlns:{pfx}="{uri}">'
        if new != tag:
            open(path, 'w', encoding='utf-8').write(xml.replace(tag, new, 1))

def fix_styles(path):
    xml = open(path, encoding='utf-8').read()
    if 'w:bidi="th-TH"' in xml: return
    head = xml.split('</w:rPrDefault>')[0]
    if '<w:lang' in head:
        i = head.rfind('<w:lang ')
        j = xml.index('/>', i) + 2
        xml = xml[:i] + '<w:lang w:val="en-US" w:eastAsia="en-US" w:bidi="th-TH"/>' + xml[j:]
    else:
        xml = xml.replace('</w:rPr></w:rPrDefault>',
            '<w:lang w:val="en-US" w:eastAsia="en-US" w:bidi="th-TH"/></w:rPr></w:rPrDefault>', 1)
    open(path, 'w', encoding='utf-8').write(xml)

src, dst = sys.argv[1], sys.argv[2]
tmp = tempfile.mkdtemp()
with zipfile.ZipFile(src) as z:
    names = z.namelist(); z.extractall(tmp)
for name in names:
    p = os.path.join(tmp, name)
    if name == 'word/styles.xml': fix_styles(p)
    elif name == 'word/document.xml' or re.match(r'word/(header|footer)\d*\.xml', name):
        process(p)
with zipfile.ZipFile(dst, 'w', zipfile.ZIP_DEFLATED) as z:
    for name in names:
        z.write(os.path.join(tmp, name), name)
print('wrote', dst)
