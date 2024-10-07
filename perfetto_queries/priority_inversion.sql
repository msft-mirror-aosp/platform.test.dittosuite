INCLUDE PERFETTO MODULE slices.with_context;

SELECT
  process_name,
  thread_name,
  name AS instruction_name,
  MIN(dur) AS duration_min_ns,
  AVG(dur) AS duration_avg_ns,
  MAX(dur) AS duration_max_ns
FROM thread_slice as slice
WHERE 1=1
  AND ( 0=1
    OR thread_name LIKE "%High_%"
    OR thread_name LIKE "%Mid_%"
    OR thread_name LIKE "%Low_%"
  )
  AND name = "lock"
GROUP BY
  process_name, thread_name, name
ORDER BY
  process_name, thread_name, name
