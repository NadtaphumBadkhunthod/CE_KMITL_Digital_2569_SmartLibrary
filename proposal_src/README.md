# proposal_src

ไฟล์ต้นทางของ `Proposal_SmartLibrary_updated.docx`

1. แก้เนื้อหาใน `build_proposal.py` (และแผนภาพใน `diagram.html` ถ้าจำเป็น)
2. แผนภาพ: เปิด `diagram.html` ใน Chrome แล้วจับภาพเป็น `diagram.png` (1400x720, scale 2)
3. สร้างไฟล์:

```
python build_proposal.py raw.docx diagram.png
python split_thai_runs.py raw.docx ../Proposal_SmartLibrary_updated.docx
```

ต้องมี `python-docx` และฟอนต์ TH Sarabun New / `split_thai_runs.py` แยก run ภาษาไทย-อังกฤษ เพื่อไม่ให้ Word ขีดเส้นแดงคำไทย
