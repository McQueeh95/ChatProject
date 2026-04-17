CREATE TABLE IF NOT EXISTS users(
	id BIGSERIAL PRIMARY KEY,
	username TEXT UNIQUE,
	password_hash BYTEA,
	public_key TEXT,
	created_at TIMESTAMPTZ DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS chats(
	id BIGSERIAL PRIMARY KEY,
	user1_id BIGINT NOT NULL, 
	user2_id BIGINT NOT NULL, 
	created_at TIMESTAMPTZ DEFAULT NOW(),
	CONSTRAINT fk_user1 FOREIGN KEY (user1_id) REFERENCES users(id),
	CONSTRAINT fk_user2 FOREIGN KEY (user2_id) REFERENCES users(id)
);

CREATE UNIQUE INDEX IF NOT EXISTS unique_chat_pair ON chats(
	LEAST(user1_id, user2_id),
	GREATEST(user1_id, user2_id)
);

CREATE TABLE IF NOT EXISTS messages(
	id BIGSERIAL PRIMARY KEY,
	chat_id BIGINT NOT NULL,
	sender_id BIGINT NOT NULL,
	encrypted_payload TEXT NOT NULL,
	created_at TIMESTAMPTZ DEFAULT NOW(),
	is_read BOOL DEFAULT FALSE,
	CONSTRAINT fk_chat_id FOREIGN KEY (chat_id) REFERENCES chats(id),
	CONSTRAINT fk_sender_id FOREIGN KEY (sender_id) REFERENCES users(id)
);

CREATE INDEX IF NOT EXISTS idx_messages_chat_id ON messages(chat_id);
CREATE INDEX IF NOT EXISTS idx_chats_user1 ON chats(user1_id);
CREATE INDEX IF NOT EXISTS idx_chats_user2 ON chats(user2_id);

CREATE TABLE IF NOT EXISTS users (
    id BIGSERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    
    auth_key VARCHAR(44) NOT NULL,
    salt VARCHAR(24) NOT NULL,
    
    public_key VARCHAR(44) NOT NULL,
    
    encrypted_vault VARCHAR(64) NOT NULL,
    vault_nonce VARCHAR(32) NOT NULL,
	
    created_at TIMESTAMPTZ DEFAULT NOW()
);