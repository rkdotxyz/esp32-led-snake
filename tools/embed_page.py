# =====================================================================
# embed_page.py
# ---------------------------------------------------------------------
# Copies controller/index.html into firmware/snake_matrix/controller_page.h
# as one long C++ string, so the ESP32 can store the page and serve it.
#
# Why? The ESP32 has no separate file storage set up in this project,
# so the page travels inside the firmware itself. Keeping the real page
# as a normal .html file means you can edit it with proper highlighting
# and preview it in a desktop browser.
#
# Run it from the repo root every time you change index.html:
#     python3 tools/embed_page.py
# =====================================================================

from pathlib import Path

# Work out the repo root from where this script lives (tools/..),
# so it works no matter which folder you run it from.
root = Path(__file__).resolve().parent.parent
source = root / "controller" / "index.html"
target = root / "firmware" / "snake_matrix" / "controller_page.h"

html = source.read_text(encoding="utf-8")

# The page is wrapped in R"rawliteral( ... )rawliteral". If the HTML
# itself contained that closing marker, the C++ string would end early.
if ')rawliteral"' in html:
    raise SystemExit("index.html contains )rawliteral\" - rename that text first")

header = f"""// =====================================================================
// controller_page.h  --  GENERATED FILE, do not edit by hand.
// Source: controller/index.html
// Regenerate with: python3 tools/embed_page.py
// =====================================================================

#pragma once

// R"rawliteral( ... )rawliteral" is a C++ "raw string": everything in
// between is kept exactly as written, so quotes and backslashes in the
// HTML need no escaping.
const char CONTROLLER_PAGE[] = R"rawliteral({html})rawliteral";
"""

target.write_text(header, encoding="utf-8")
print(f"Wrote {target.relative_to(root)} ({len(html):,} characters of HTML)")
