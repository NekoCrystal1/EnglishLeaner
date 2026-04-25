DELETE FROM book_word
WHERE book_id IN (
    SELECT id
    FROM book
    WHERE book_type = 'WORD'
      AND source_type = 'SYSTEM'
      AND owner_user_id IS NULL
      AND item_count <= 8
      AND is_deleted = 0
);

DELETE FROM book
WHERE book_type = 'WORD'
  AND source_type = 'SYSTEM'
  AND owner_user_id IS NULL
  AND item_count <= 8
  AND is_deleted = 0
  AND NOT EXISTS (
      SELECT 1
      FROM user_word_plan
      WHERE user_word_plan.book_id = book.id
  );

UPDATE book
SET status = 'ARCHIVED',
    is_deleted = 1,
    remark = 'Removed demo 8-word system book'
WHERE book_type = 'WORD'
  AND source_type = 'SYSTEM'
  AND owner_user_id IS NULL
  AND item_count <= 8
  AND is_deleted = 0;

DELETE FROM vocabulary_word
WHERE normalized_word IN ('apple', 'car', 'happy', 'big', 'run', 'blue', 'cat', 'sun')
  AND NOT EXISTS (SELECT 1 FROM book_word WHERE book_word.word_id = vocabulary_word.id)
  AND NOT EXISTS (SELECT 1 FROM user_word_state WHERE user_word_state.word_id = vocabulary_word.id)
  AND NOT EXISTS (SELECT 1 FROM word_study_event WHERE word_study_event.word_id = vocabulary_word.id);

UPDATE vocabulary_word
SET is_deleted = 1,
    remark = 'Removed legacy demo vocabulary'
WHERE normalized_word IN ('apple', 'car', 'happy', 'big', 'run', 'blue', 'cat', 'sun')
  AND remark IS NULL;

DELETE FROM vocabulary
WHERE LOWER(word) IN ('apple', 'car', 'happy', 'big', 'run', 'blue', 'cat', 'sun')
  AND NOT EXISTS (SELECT 1 FROM study_log WHERE study_log.word_id = vocabulary.id);

UPDATE vocabulary
SET is_deleted = 1,
    remark = 'Removed legacy demo vocabulary'
WHERE LOWER(word) IN ('apple', 'car', 'happy', 'big', 'run', 'blue', 'cat', 'sun')
  AND is_deleted = 0;
