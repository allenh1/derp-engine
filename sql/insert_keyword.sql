DELIMITER $$
DROP PROCEDURE IF EXISTS InsertKeyword$$

CREATE PROCEDURE InsertKeyword(
 IN _url VARCHAR(500),
 IN _word VARCHAR(100),
 IN _times INT,
 OUT success BOOLEAN)
BEGIN
 -- Count the urls that exist
 DECLARE X, Y, Z, kid, wid, pid INT DEFAULT 0;
 SELECT DISTINCT count(url) AS count INTO X FROM websites WHERE url = _url;
 SELECT DISTINCT count(keyword) AS count INTO Y FROM keywords WHERE keyword = _word;
 -- Check that X is zero to avoid recreation.
 IF X = 0 THEN
  SET success = 0;
 ELSE
  -- Check that the keyword exists!
  IF Y = 0 THEN
    -- create the keyword if not exists
	INSERT INTO keywords(keyword) VALUES(_word);
  END IF;
  -- Find the keyword
  SELECT DISTINCT keyword_id AS count INTO kid FROM keywords WHERE keyword = _word;
  -- Find the website
  SELECT DISTINCT website_id AS count INTO wid FROM websites WHERE url = _url;
  -- create the relation
  INSERT INTO website_keyword_relation(website_id, keyword_id, times_used)
    VALUES(wid, kid, _times);
  -- mark this transaction as successfull.    
  SET success = 1;
 END IF;
END$$
DELIMITER ;
