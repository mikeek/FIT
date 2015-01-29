DROP TABLE objednavka CASCADE CONSTRAINTS;
DROP TABLE spravce CASCADE CONSTRAINTS;
DROP TABLE zakaznik CASCADE CONSTRAINTS;
DROP TABLE skicak CASCADE CONSTRAINTS;
DROP TABLE pastelky CASCADE CONSTRAINTS;
DROP TABLE zbozi CASCADE CONSTRAINTS;
DROP TABLE dodavatel CASCADE CONSTRAINTS;
DROP TABLE kosik CASCADE CONSTRAINTS;

DROP TABLE obj_obsahuje CASCADE CONSTRAINTS;
DROP TABLE kos_obsahuje CASCADE CONSTRAINTS;
DROP TABLE ohodnotil CASCADE CONSTRAINTS;
DROP TABLE nabizi CASCADE CONSTRAINTS;

DROP VIEW vsechny_objednavky;
DROP VIEW pastelky_info;
DROP VIEW skicak_info;
DROP VIEW kosik_info;
DROP VIEW objednavka_info;

DROP SEQUENCE seq_spr_id;
DROP SEQUENCE seq_zak_id;
DROP SEQUENCE seq_zb_id;
DROP SEQUENCE seq_kos_id;
DROP SEQUENCE seq_obj_id;
DROP SEQUENCE seq_dod_id;

DROP PROCEDURE kos_insert;
DROP PROCEDURE add_all;

/* neni potreba  
DROP INDEX ind_zak; 
*/

/* neni potreba - create or replace  
DROP TRIGGER aktualizuj_sklad; 
*/

CREATE TABLE objednavka (
    obj_id INTEGER NOT NULL,
    zak_id INTEGER NOT NULL,
    doruc_adresa VARCHAR(40) NOT NULL,
    doprava VARCHAR(10),
    obj_cena INTEGER NOT NULL
);

CREATE TABLE spravce (
    spr_id INTEGER NOT NULL,
    login VARCHAR(20) UNIQUE NOT NULL,
    heslo VARCHAR(20) NOT NULL
);

CREATE TABLE zakaznik (
    zak_id INTEGER NOT NULL,
    login VARCHAR(20) UNIQUE NOT NULL,
    heslo VARCHAR(20) NOT NULL,
    jmeno VARCHAR(15) NOT NULL,
    prijmeni VARCHAR(15) NOT NULL,
    kontakt VARCHAR(30),
    adresa VARCHAR(40) NOT NULL
);

CREATE TABLE skicak (
    gramaz INTEGER NOT NULL,
    velikost VARCHAR(3) NOT NULL,
    papiru INTEGER NOT NULL,
    zb_id INTEGER NOT NULL
);

CREATE TABLE pastelky (
    typ VARCHAR(10) NOT NULL,
    pocet INTEGER NOT NULL,
    delka INTEGER NOT NULL,
    tvrdost VARCHAR(3) NOT NULL,
    zb_id INTEGER NOT NULL
);

CREATE TABLE zbozi (
    zb_id INTEGER NOT NULL,
    vyrobce VARCHAR(20) NOT NULL,
    skladem INTEGER NOT NULL,
    zb_cena INTEGER NOT NULL
);

CREATE TABLE dodavatel (
    dod_id INTEGER NOT NULL,
    nazev VARCHAR(20) NOT NULL,
    kontakt VARCHAR(30),
    adresa VARCHAR(40) NOT NULL,
    zeme VARCHAR(20) NOT NULL
);

CREATE TABLE kosik (
    kos_id INTEGER NOT NULL,
    zak_id INTEGER NOT NULL,
    castka INTEGER NOT NULL
);

CREATE TABLE obj_obsahuje (
    obj_id INTEGER NOT NULL,
    zb_id INTEGER NOT NULL,
    pocet INTEGER NOT NULL,
    cena INTEGER NOT NULL
);

CREATE TABLE kos_obsahuje (
    kos_id INTEGER NOT NULL,
    zb_id INTEGER NOT NULL,
    kusu INTEGER NOT NULL,
    cena INTEGER NOT NULL
);

CREATE TABLE ohodnotil (
    zak_id INTEGER NOT NULL,
    zb_id INTEGER NOT NULL,
    znamka INTEGER NOT NULL
);

CREATE TABLE nabizi (
    dod_id INTEGER NOT NULL,
    zb_id INTEGER NOT NULL,
    termin INTEGER NOT NULL,
    cena INTEGER NOT NULL
);

ALTER TABLE spravce ADD CONSTRAINT PK_spr_id PRIMARY KEY (spr_id);
ALTER TABLE zakaznik ADD CONSTRAINT PK_zak_id PRIMARY KEY (zak_id);
ALTER TABLE objednavka ADD CONSTRAINT PK_obj_id PRIMARY KEY (obj_id);
ALTER TABLE objednavka ADD CONSTRAINT FK_obj_id FOREIGN KEY (zak_id) REFERENCES zakaznik(zak_id);

ALTER TABLE zbozi ADD CONSTRAINT PK_zb_id PRIMARY KEY (zb_id);

ALTER TABLE skicak ADD CONSTRAINT FK_s_zb_id FOREIGN KEY(zb_id) REFERENCES zbozi(zb_id);
ALTER TABLE pastelky ADD CONSTRAINT FK_p_zb_id FOREIGN KEY(zb_id) REFERENCES zbozi(zb_id);

ALTER TABLE dodavatel ADD CONSTRAINT PK_dod_id PRIMARY KEY (dod_id);
ALTER TABLE kosik ADD CONSTRAINT PK_kos_id PRIMARY KEY (kos_id);
ALTER TABLE kosik ADD CONSTRAINT FK_zak_id FOREIGN KEY (zak_id) REFERENCES zakaznik(zak_id);

ALTER TABLE obj_obsahuje ADD CONSTRAINT FK_o_obj_id FOREIGN KEY (obj_id) REFERENCES objednavka(obj_id);
ALTER TABLE obj_obsahuje ADD CONSTRAINT FK_o_zb_id FOREIGN KEY (zb_id) REFERENCES zbozi(zb_id);
ALTER TABLE obj_obsahuje ADD CONSTRAINT PK_obj_obs PRIMARY KEY (obj_id, zb_id);

ALTER TABLE kos_obsahuje ADD CONSTRAINT FK_k_kos_id FOREIGN KEY (kos_id) REFERENCES kosik(kos_id);
ALTER TABLE kos_obsahuje ADD CONSTRAINT FK_k_zb_id FOREIGN KEY (zb_id) REFERENCES zbozi(zb_id);
ALTER TABLE kos_obsahuje ADD CONSTRAINT PK_kos_obs PRIMARY KEY (kos_id, zb_id);

ALTER TABLE ohodnotil ADD CONSTRAINT FK_oh_zak_id FOREIGN KEY (zak_id) REFERENCES zakaznik(zak_id);
ALTER TABLE ohodnotil ADD CONSTRAINT FK_oh_zb_id FOREIGN KEY (zb_id) REFERENCES zbozi(zb_id);
ALTER TABLE ohodnotil ADD CONSTRAINT PK_ohodnotil PRIMARY KEY(zak_id, zb_id);

ALTER TABLE nabizi ADD CONSTRAINT FK_n_dod_id FOREIGN KEY (dod_id) REFERENCES dodavatel(dod_id);
ALTER TABLE nabizi ADD CONSTRAINT FK_n_zb_id FOREIGN KEY (zb_id) REFERENCES zbozi(zb_id);
ALTER TABLE nabizi ADD CONSTRAINT PK_nabizi PRIMARY KEY(dod_id, zb_id);

CREATE SEQUENCE seq_spr_id START WITH 1 INCREMENT BY 1;
CREATE SEQUENCE seq_zak_id START WITH 1 INCREMENT BY 1;
CREATE SEQUENCE seq_zb_id  START WITH 1 INCREMENT BY 1;
CREATE SEQUENCE seq_kos_id START WITH 1 INCREMENT BY 1;
CREATE SEQUENCE seq_obj_id START WITH 1 INCREMENT BY 1;
CREATE SEQUENCE seq_dod_id START WITH 1 INCREMENT BY 1;

INSERT INTO spravce VALUES(seq_spr_id.nextval, 'admin', 'admin');
INSERT INTO zakaznik
VALUES(seq_zak_id.nextval, 'xkozub03',    'tajne',   'Michal',    'Kozubik', 'xkozub03@stud.fit.vutbr.cz', 'Dukelska trida 523/89A, Brno');
INSERT INTO zakaznik
VALUES(seq_zak_id.nextval, 'xokno03',     'heslo',   'Marcel',    'Okno',  'marokno@ema.il',         'Horni 6, Veleves');
INSERT INTO zakaznik
VALUES(seq_zak_id.nextval, 'xdvere02',    'abcd',    'Bronislav', 'Dvere', 'brona@ema.il',           'Lhanice 9');
INSERT INTO zakaznik
VALUES(seq_zak_id.nextval, 'xstul05',     '1234',    'Dobromil',  'Stul',  'stolek@goo.gle',         'Bozetechova 1, Brno');
INSERT INTO zakaznik
VALUES(seq_zak_id.nextval, 'xlampa12',    'zarovka', 'Hugo',      'Lampa', 'boss@kontakt.org',       'Poulicni 42, Brno');

INSERT INTO zbozi VALUES(seq_zb_id.nextval, 'Pastelkarna',     30, 33);
INSERT INTO zbozi VALUES(seq_zb_id.nextval, 'Skicnovik',       2,  25);
INSERT INTO zbozi VALUES(seq_zb_id.nextval, 'Koh i Nor i Dan', 0,  30);
INSERT INTO zbozi VALUES(seq_zb_id.nextval, 'Pastelkarna',     11, 80);
INSERT INTO zbozi VALUES(seq_zb_id.nextval, 'Skicovnik',       0,  50);

INSERT INTO pastelky VALUES('trojhranne', 10, 12, 'H2', 1);
INSERT INTO pastelky VALUES('petihranne', 8,  14, 'B1', 3);
INSERT INTO pastelky VALUES('kulate',     16, 12, 'H3', 4);

INSERT INTO skicak VALUES(180, 'A4', 20, 2);
INSERT INTO skicak VALUES(130, 'A4', 20, 5);

INSERT INTO kosik VALUES(seq_kos_id.nextval, 1, 150);
INSERT INTO kosik VALUES(seq_kos_id.nextval, 2, 100);
INSERT INTO kosik VALUES(seq_kos_id.nextval, 3, 0);
INSERT INTO kosik VALUES(seq_kos_id.nextval, 4, 0);
INSERT INTO kosik VALUES(seq_kos_id.nextval, 5, 800);

INSERT INTO objednavka VALUES(seq_obj_id.nextval, 1, 'Bozetechova 2, Brno', 'ano', 150);
INSERT INTO objednavka VALUES(seq_obj_id.nextval, 5, 'Poulicni 42, Brno',   'ne',  100);
INSERT INTO objednavka VALUES(seq_obj_id.nextval, 1, 'Dukelska trida 523/89A, Brno', 'ne', 50);

INSERT INTO dodavatel
VALUES(seq_dod_id.nextval, 'Pastelkarna',     'pastel@kar.na', 'Ostrouhana 3, Pastelkov', 'Pastelkoland');
INSERT INTO dodavatel
VALUES(seq_dod_id.nextval, 'Skicovnik',       'skic@ovnik.jo', 'U skicovniku 1, Skica',    'Papirova republika');
INSERT INTO dodavatel
VALUES(seq_dod_id.nextval, 'Koh i Nor i Dan', 'kohi@nori.dan', 'Nagy van 30, Kokinen',    'Kohish');

INSERT INTO nabizi VALUES(1, 1, 10, 50);
INSERT INTO nabizi VALUES(1, 4, 10, 20);
INSERT INTO nabizi VALUES(2, 2, 5,  30);
INSERT INTO nabizi VALUES(3, 3, 30, 60);
INSERT INTO nabizi VALUES(2, 5, 5,  30);

INSERT INTO obj_obsahuje VALUES(1, 5, 3, 150);
INSERT INTO obj_obsahuje VALUES(2, 1, 3, 99);
INSERT INTO obj_obsahuje VALUES(3, 2, 2, 50);

INSERT INTO kos_obsahuje VALUES(1, 5, 3,  150);
INSERT INTO kos_obsahuje VALUES(1, 4, 2,  160);
INSERT INTO kos_obsahuje VALUES(2, 1, 3,  99);
INSERT INTO kos_obsahuje VALUES(3, 4, 10, 800);

CREATE VIEW vsechny_objednavky AS
SELECT login, zb_id, doruc_adresa, doprava, cena
FROM zakaznik NATURAL JOIN objednavka NATURAL JOIN obj_obsahuje
NATURAL JOIN zbozi;

CREATE UNIQUE INDEX ind_zak ON zakaznik(zak_id, login);

CREATE VIEW pastelky_info AS
SELECT *
FROM pastelky NATURAL JOIN zbozi;

CREATE VIEW skicak_info AS
SELECT *
FROM skicak NATURAL JOIN zbozi;

CREATE VIEW kosik_info AS
SELECT zb_id, kos_id, kusu, cena, vyrobce, skladem, login
FROM kosik K NATURAL JOIN kos_obsahuje NATURAL JOIN zbozi, zakaznik Z
WHERE K.zak_id = Z.zak_id;

CREATE PROCEDURE kos_insert(id_kose INTEGER, id_zbozi INTEGER, pocet INTEGER, castka INTEGER) AS
    v_kosi INTEGER;
  begin
		SELECT COUNT(*) INTO v_kosi FROM kos_obsahuje WHERE kos_id = id_kose AND zb_id = id_zbozi;
    IF v_kosi = 0 THEN
      INSERT INTO kos_obsahuje VALUES(
        id_kose, id_zbozi, pocet, castka);
    ELSE
      UPDATE kos_obsahuje
      SET kusu = (kusu + pocet), cena = (cena + castka)
      WHERE kos_id = id_kose AND zb_id = id_zbozi;
    END IF;
    COMMIT;
  END;
/

CREATE VIEW objednavka_info AS
SELECT obj_id, Z.zak_id, obj_cena, doruc_adresa, doprava, login
FROM objednavka O, zakaznik Z
WHERE O.zak_id = Z.zak_id;

CREATE PROCEDURE add_all(id_kose INTEGER, id_objednavky INTEGER) AS
 cursor c_kos IS 
    SELECT zb_id, kusu, cena
    FROM kosik_info
    WHERE kos_id = id_kose;

BEGIN
  FOR rec IN c_kos
  LOOP
    INSERT INTO obj_obsahuje VALUES (
      id_objednavky, rec.zb_id, rec.kusu, rec.cena);
  END LOOP;
END;
/
ALTER SESSION SET PLSCOPE_SETTINGS = 'IDENTIFIERS:NONE';

CREATE OR REPLACE TRIGGER aktualizuj_sklad 
AFTER INSERT ON obj_obsahuje 
REFERENCING NEW AS objednano
FOR EACH ROW
BEGIN
  UPDATE zbozi SET skladem = skladem - :objednano.pocet
  WHERE zbozi.zb_id = :objednano.zb_id;
END;
/
ALTER SESSION SET PLSCOPE_SETTINGS = 'IDENTIFIERS:ALL'; 

GRANT INSERT,SELECT,UPDATE        ON zakaznik     TO xkozub03;
GRANT INSERT,SELECT,UPDATE        ON dodavatel    TO xkozub03;
GRANT DELETE,INSERT,SELECT,UPDATE ON kos_obsahuje TO xkozub03;
GRANT INSERT,SELECT,UPDATE        ON obj_obsahuje TO xkozub03;
GRANT INSERT,SELECT,UPDATE        ON objednavka   TO xkozub03;
GRANT INSERT,SELECT,UPDATE        ON ohodnotil    TO xkozub03;

/* nelze na sebe 
REVOKE DELETE,INSERT,UPDATE       ON zbozi      FROM xkozub03;
REVOKE DELETE,INSERT,UPDATE       ON pastelky   FROM xkozub03;
REVOKE DELETE,INSERT,UPDATE       ON skicak     FROM xkozub03;
*/
COMMIT;