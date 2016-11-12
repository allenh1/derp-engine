CREATE TABLE IF NOT EXISTS websites(
  -- primary key for the website
  website_id INTEGER NOT NULL AUTO_INCREMENT,
  -- url for the website
  url VARCHAR(512) NOT NULL,
  PRIMARY KEY(website_id)
);

CREATE TABLE IF NOT EXISTS keywords(
  -- primary key for keyword
  keyword_id INTEGER NOT NULL AUTO_INCREMENT,
  -- text for the keyword
  keyword VARCHAR(100) NOT NULL,
  PRIMARY KEY(keyword_id)
);

CREATE TABLE IF NOT EXISTS website_keyword_relation(
  -- website tag
  website_id INTEGER NOT NULL,
  -- keyword tag
  keyword_id INTEGER NOT NULL,
  -- number of times the word was used
  times_used UNSIGNED INTEGER NOT NULL,

  -- foreign key setup
  FOREIGN KEY(website_id) REFERENCES websites(website_id),
  FOREIGN KEY(keyword_id) REFERENCES keywords(keyword_id)
);

