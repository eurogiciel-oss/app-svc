PRAGMA journal_mode = PERSIST;

CREATE TABLE IF NOT EXISTS appsvc (
        operation TEXT,
        mime_type TEXT DEFAULT 'NULL',
        uri TEXT DEFAULT 'NULL',
        pkg_name TEXT,
        PRIMARY KEY (operation,mime_type,uri)
);


