"""
Python script to add kotex package in refman

:Author: jupiterbjy@gmail.com
"""

import pathlib


INSERT_TGT = r"\usepackage{kotex}"
LINE_UNDER = "%===== C O N T E N T S ====="


TGT = pathlib.Path.cwd() / "doxy_output" / "latex" / "refman.tex"

if not TGT.exists():
    input(f"[AddKotex] No file found at {TGT.as_posix()} - Press enter to continue:")
    exit(1)

print("[AddKotex] Injecting kotex dependancy!")

content = TGT.read_text("utf8")
content = content.replace(LINE_UNDER, "\n".join((LINE_UNDER, INSERT_TGT)), 1)

TGT.write_text(content, "utf8")
