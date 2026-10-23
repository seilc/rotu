#!/usr/bin/env python3
"""Print a Discord message (JSON payload) with the progress from objdiff reports, as a list.

Usage: progress.py CUR_DIR [PREV_DIR]
Prints nothing when no displayed number changed since PREV_DIR.
Each directory holds GIQE78_report/report.json, as downloaded from the Build workflow's artifacts.
"""
import json
import sys
from pathlib import Path

VERSION = "GIQE78"
MEASURES = [("Code", "matched_code", "total_code"), ("Data", "matched_data", "total_data")]


def load(d, ver):
    p = Path(d) / f"{ver}_report" / "report.json"
    if not p.exists():
        return None
    m = json.loads(p.read_text())["measures"]
    return {k: int(v) for k, v in m.items() if not k.endswith("_percent")}


def pct(m, key, total):
    return 100.0 * m.get(key, 0) / m[total] if m.get(total) else 0.0


def main():
    args = sys.argv[1:]
    cur_dir, prev_dir = args[0], (args[1] if len(args) > 1 else None)
    cur = load(cur_dir, VERSION)
    prev = load(prev_dir, VERSION) if prev_dir else None
    lines = ["**Progress**"]
    changed = not prev
    for name, key, total in MEASURES:
        line = f"- {name}: **{pct(cur, key, total):.2f}%**"
        if prev:
            delta = pct(cur, key, total) - pct(prev, key, total)
            changed |= round(delta, 2) != 0
            line += f" ({delta:+.2f}%)"
        lines.append(line)
    units = cur.get("complete_units", 0)
    line = f"- Files: **{units}/{cur.get('total_units', 0)}**"
    if prev:
        delta = units - prev.get("complete_units", 0)
        changed |= delta != 0
        line += f" ({delta:+d})"
    lines.append(line)
    if not prev:
        lines.append("(no previous report to compare with)")
    if changed:
        print(json.dumps({"username": "Underminer", "content": "\n".join(lines)}))


if __name__ == "__main__":
    main()
