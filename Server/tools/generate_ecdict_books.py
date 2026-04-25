import csv
import hashlib
import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOURCE_CSV = ROOT / "build" / "ecdict" / "ecdict.csv"
OUTPUT_DIR = ROOT / "src" / "main" / "resources" / "vocabulary" / "ecdict"
SOURCE_URL = "https://github.com/skywind3000/ECDICT"
RAW_CSV_URL = "https://raw.githubusercontent.com/skywind3000/ECDICT/master/ecdict.csv"

BOOKS = [
    {
        "key": "high-school",
        "title": "高中核心词库",
        "level": "HIGH_SCHOOL",
        "tag": "gk",
        "unitSize": 20,
        "difficulty": 2,
    },
    {
        "key": "cet4",
        "title": "CET4核心词库",
        "level": "CET4",
        "tag": "cet4",
        "unitSize": 20,
        "difficulty": 3,
    },
    {
        "key": "cet6",
        "title": "CET6核心词库",
        "level": "CET6",
        "tag": "cet6",
        "unitSize": 20,
        "difficulty": 4,
    },
    {
        "key": "postgraduate",
        "title": "考研核心词库",
        "level": "POSTGRADUATE",
        "tag": "ky",
        "unitSize": 20,
        "difficulty": 4,
    },
]

WORD_RE = re.compile(r"^[A-Za-z][A-Za-z-]{1,63}$")
WHITESPACE_RE = re.compile(r"\s+")


def normalize_space(value):
    value = (value or "").replace("\\n", " ")
    return WHITESPACE_RE.sub(" ", value.strip())


def truncate(value, length):
    value = normalize_space(value)
    if len(value) <= length:
        return value
    return value[: length - 1].rstrip() + "…"


def int_or_none(value):
    try:
        parsed = int(value or 0)
    except ValueError:
        return None
    return parsed or None


def row_is_usable(row):
    word = normalize_space(row.get("word"))
    translation = normalize_space(row.get("translation"))
    return bool(WORD_RE.match(word)) and bool(translation)


def rank_key(row):
    bnc = int_or_none(row.get("bnc")) or 999_999
    frq = int_or_none(row.get("frq")) or 999_999
    return bnc, frq, normalize_space(row.get("word")).lower()


def load_rows():
    if not SOURCE_CSV.exists():
        raise SystemExit(
            f"Missing {SOURCE_CSV}. Download ECDICT first:\n"
            f"  Invoke-WebRequest -Uri {RAW_CSV_URL} -OutFile {SOURCE_CSV}"
        )

    rows = []
    with SOURCE_CSV.open("r", encoding="utf-8", newline="") as handle:
        for row in csv.DictReader(handle):
            if row_is_usable(row):
                rows.append(row)
    return rows


def word_from_row(row, difficulty):
    word = truncate(row.get("word"), 128)
    tags = normalize_space(row.get("tag")).split()
    item = {
        "word": word,
        "normalizedWord": word.lower(),
        "phonetic": truncate(row.get("phonetic"), 128),
        "translation": truncate(row.get("translation"), 256),
        "partOfSpeech": truncate(row.get("pos"), 64),
        "difficulty": difficulty,
        "tags": tags,
        "definition": truncate(row.get("definition"), 700),
        "exchange": truncate(row.get("exchange"), 300),
        "bnc": int_or_none(row.get("bnc")),
        "frq": int_or_none(row.get("frq")),
    }
    return {key: value for key, value in item.items() if value not in (None, "", [])}


def book_words(rows, book):
    tag = book["tag"]
    selected = [
        row for row in rows
        if tag in normalize_space(row.get("tag")).split()
    ]
    selected.sort(key=rank_key)

    seen = set()
    words = []
    for row in selected:
        normalized = normalize_space(row.get("word")).lower()
        if normalized in seen:
            continue
        seen.add(normalized)
        words.append(word_from_row(row, book["difficulty"]))
    return words


def content_hash(payload):
    encoded = json.dumps(payload, ensure_ascii=False, sort_keys=True, separators=(",", ":")).encode("utf-8")
    return hashlib.sha256(encoded).hexdigest()


def write_book(book, words):
    payload = {
        "key": book["key"],
        "title": book["title"],
        "bookType": "WORD",
        "sourceType": "SYSTEM",
        "language": "en",
        "level": book["level"],
        "source": "ECDICT",
        "sourceUrl": SOURCE_URL,
        "rawCsvUrl": RAW_CSV_URL,
        "license": "MIT",
        "tag": book["tag"],
        "unitSize": book["unitSize"],
        "itemCount": len(words),
        "words": words,
    }
    payload["contentHash"] = content_hash(payload)
    path = OUTPUT_DIR / f"{book['key']}.json"
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    return {
        "key": book["key"],
        "title": book["title"],
        "level": book["level"],
        "path": f"vocabulary/ecdict/{book['key']}.json",
        "itemCount": len(words),
        "contentHash": payload["contentHash"],
    }


def main():
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    rows = load_rows()
    index = {
        "source": "ECDICT",
        "sourceUrl": SOURCE_URL,
        "rawCsvUrl": RAW_CSV_URL,
        "license": "MIT",
        "books": [],
    }
    for book in BOOKS:
        words = book_words(rows, book)
        index["books"].append(write_book(book, words))
        print(f"{book['key']}: {len(words)} words")

    (OUTPUT_DIR / "index.json").write_text(
        json.dumps(index, ensure_ascii=False, indent=2) + "\n",
        encoding="utf-8",
    )
    print(f"wrote {OUTPUT_DIR}")


if __name__ == "__main__":
    main()
