# design_doc_src

ไฟล์ต้นทางของ `DesignDocument_SmartLibrary.docx`

1. แก้เนื้อหาใน `build_design_doc.py` / แผนภาพใน `fig_*.html`
2. แผนภาพที่ใช้: `fig_hierarchy`, `fig_system`, `fig_board1v`, `fig_board2v`, `fig_fsm3` เปิดใน Chrome แล้วจับภาพเป็น `.png` กว้าง 1000 px (scale 3) ส่วน `fig_board1`, `fig_board2`, `fig_fsm`, `fig_fsm2` เป็นฉบับเก่า
   ตราสถาบันบนหน้าปกอยู่ใน `kmitl_seal.png`
3. สร้างไฟล์:

```
python build_design_doc.py raw.docx
python ../proposal_src/split_thai_runs.py raw.docx ../DesignDocument_SmartLibrary.docx
```

4. เปิดใน Word แล้วคลิกขวาที่สารบัญ → Update Field (ตัวเลขหน้า) ก่อน export PDF
