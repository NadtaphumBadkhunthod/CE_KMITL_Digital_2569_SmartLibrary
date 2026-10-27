# design_doc_src

ไฟล์ต้นทางของ `DesignDocument_SmartLibrary.docx`

1. แก้เนื้อหาใน `build_design_doc.py` / แผนภาพใน `fig_*.html`
2. แผนภาพ: เปิด `fig_*.html` ใน Chrome แล้วจับภาพเป็น `fig_*.png` (scale 2)
3. สร้างไฟล์:

```
python build_design_doc.py raw.docx
python ../proposal_src/split_thai_runs.py raw.docx ../DesignDocument_SmartLibrary.docx
```

4. เปิดใน Word แล้วคลิกขวาที่สารบัญ → Update Field (ตัวเลขหน้า) ก่อน export PDF
