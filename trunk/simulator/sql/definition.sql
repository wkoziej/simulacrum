--insert into fields_articles_snapshots (field_snapshot_id, article_id, field_stock, market_stock, sell_price, buy_price)  values (1, 2, 3, 4, 5,6); 

--insert into creatures_products_snapshots (creature_snapshot_id, produced, needs)  values (:1, :2, :3);

--insert into creatures_resources_snapshots (creature_snapshot_id, use)  values (:1, :2); 
--insert into worlds_snapshots (world_run_id, snapshot_time) values (1, date('now'));

--select * from SQLITE_SEQUENCE;

select * from fields_snapshots;

drop table if exists WORLDS;
create table WORLDS (
    ID INTEGER CONSTRAINT WORLDS_PK PRIMARY KEY ASC AUTOINCREMENT NOT NULL,
    JSON_DEF TEXT
);

drop table if exists WORLDS_RUNS;
create table WORLDS_RUNS (
    ID INTEGER CONSTRAINT WORLDS_RUNS_PK PRIMARY KEY ASC AUTOINCREMENT NOT NULL,
    WORLD_ID INTEGER CONSTRAINT WORLDS_FK REFERENCES WORLDS (ID) ON DELETE CASCADE,
    DATE_START DATE_TEXT,
    DATE_STOP DATE_TEXT,
    STOP_CASE TEXT
);

drop table  if exists WORLDS_SNAPSHOTS;
   create table WORLDS_SNAPSHOTS (
       ID INTEGER CONSTRAINT WORLDS_SNAPSHOTS_PK PRIMARY KEY ASC AUTOINCREMENT NOT NULL,
       WORLD_RUN_ID INTEGER CONSTRAINT WORLDS_RUNS_FK REFERENCES WORLDS_RUNS (ID) ON DELETE CASCADE,
       SNAPSHOT_TIME DATE_TEXT,
       DESCRIPTION TEXT
 );
  
 drop table  if exists  FIELDS_SNAPSHOTS;
CREATE TABLE FIELDS_SNAPSHOTS (
    ID INTEGER CONSTRAINT FIELDS_SNAPSHOTS_PK PRIMARY KEY ASC AUTOINCREMENT NOT NULL,
    WORLD_SNAPSHOT_ID INTEGER CONSTRAINT WORLDS_SNAPSHOTS_FK REFERENCES WORLDS_SNAPSHOTS (ID) ON DELETE CASCADE,
    LAG REAL NOT NULL
);

drop table  if exists ARTICLES;
   create table ARTICLES (
       ID INTEGER CONSTRAINT ARTICLES_PK PRIMARY KEY ASC AUTOINCREMENT NOT NULL,
       NAME TEXT
 );
/*
drop table  if exists  RESOURCES;
   create table RESOURCES (
       ID INTEGER CONSTRAINT RESOURCES_PK PRIMARY KEY ASC AUTOINCREMENT NOT NULL,
       WORLD_ID INTEGER CONSTRAINT WORLDS_FK REFERENCES WORLDS (ID) ON DELETE CASCADE,      
       NAME TEXT
 );
   */
  drop table  if exists  POPULATIONS_SNAPSHOTS;
  create table POPULATIONS_SNAPSHOTS (
       ID INTEGER CONSTRAINT POPULATIONS_SNAPSHOTS_PK PRIMARY KEY ASC AUTOINCREMENT NOT NULL,
       FIELD_SNAPSHOT_ID INTEGER CONSTRAINT FIELDS_SNAPSHOTS_FK REFERENCES FIELDS_SNAPSHOTS(ID) ON DELETE CASCADE,
       SPECIES TEXT
 );
 
 drop table  if exists CREATURES_SNAPSHOTS;
   create table CREATURES_SNAPSHOTS (
       ID INTEGER CONSTRAINT CREATURES_SNAPSHOTS_PK PRIMARY KEY ASC AUTOINCREMENT NOT NULL,
       POPULATION_SNAPSHOT_ID INTEGER CONSTRAINT POPULATIONS_SNAPSHOTS_FK REFERENCES POPULATIONS_SNAPSHOTS(ID) ON DELETE CASCADE,
       AGE INTEGER,
       OBJECTIVE_VALUE REAL,
       WALLET REAL,
       GENOME TEXT
 );
 
 drop table  if exists CREATURES_ARTICLES_SNAPSHOTS;
 create table CREATURES_ARTICLES_SNAPSHOTS (
	ID INTEGER  PRIMARY KEY ASC AUTOINCREMENT NOT NULL,
       CREATURE_SNAPSHOT_ID INTEGER CONSTRAINT CREATURES_SNAPSHOTS_FK REFERENCES CREATURES_SNAPSHOTS(ID) ON DELETE CASCADE,
ARTICLE_ID INT,
       STOCK INT,
       CHANGED INT
 );
 
 /*
 drop table  if exists CREATURES_RESOURCES_SNAPSHOTS;
 create table CREATURES_RESOURCES_SNAPSHOTS (
 	ID INTEGER  PRIMARY KEY ASC AUTOINCREMENT NOT NULL,
       CREATURE_SNAPSHOT_ID INTEGER CONSTRAINT CREATURES_SNAPSHOTS_FK REFERENCES CREATURES_SNAPSHOTS(ID) ON DELETE CASCADE,
       RESOURCE_ID INT,
       USED REAL
  );
  */
  
  drop table  if exists FIELDS_ARTICLES_SNAPSHOTS;
   create table FIELDS_ARTICLES_SNAPSHOTS (
	ID INTEGER  PRIMARY KEY ASC AUTOINCREMENT NOT NULL,
       FIELD_SNAPSHOT_ID INTEGER CONSTRAINT FIELDS_SNAPSHOTS_FK REFERENCES FIELDS_SNAPSHOTS(ID) ON DELETE CASCADE,
       ARTICLE_ID INT,
       SELL_PRICE REAL,
       BUY_PRICE REAL,
       FIELD_STOCK INT,
       MARKET_STOCK INT,
       QUERIES_COUNT INT    
 );
 

  
 drop view V_WORLDS_RUNS;
CREATE VIEW V_WORLDS_RUNS AS select * from WORLDS_RUNS;

drop view V_WORLDS_SNAPSHOTS;
CREATE VIEW V_WORLDS_SNAPSHOTS AS select * from WORLDS_SNAPSHOTS;


drop view V_FIELDS_SNAPSHOTS;
create view V_FIELDS_SNAPSHOTS AS 
select * from FIELDS_SNAPSHOTS;



drop view V_POPULATIONS_SNAPSHOTS;
create view V_POPULATIONS_SNAPSHOTS as select * from POPULATIONS_SNAPSHOTS;

drop view v_fields_articles_snapshots;
create view v_fields_articles_snapshots as 
select s.id, s.field_snapshot_id,  a.name,  s.field_stock, s.market_stock,  s.queries_count, s.sell_price, s.buy_price
 from fields_articles_snapshots s inner join articles a on a.id = s.article_id;


drop view v_creatures_snapshots;
create view v_creatures_snapshots as select * from creatures_snapshots;

drop view v_creatures_articles_snapshots;
create  view v_creatures_articles_snapshots as 
select s.id, s.creature_snapshot_id, a.name, s.stock, s.changed from creatures_articles_snapshots  s inner join articles a on a.id = s.article_id;