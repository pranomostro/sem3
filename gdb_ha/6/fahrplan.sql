DROP TABLE IF EXISTS Fahrplan;

CREATE TABLE Fahrplan (
    Von VARCHAR(100) NOT NULL,
    Nach VARCHAR(100) NOT NULL,
    Linie VARCHAR(10) NOT NULL,
    Abfahrt TIME NOT NULL,
    Ankunft TIME NOT NULL
);

insert into Fahrplan values
    ('Garching, Forschungszentrum',     'Garching',                     'U6',  '09:06:00', '09:09:00'),
    ('Garching',                        'Garching-Hochbrück',           'U6',  '09:09:00', '09:11:00'),
    ('Garching-Hochbrück',              'Fröttmaning',                  'U6',  '09:11:00', '09:15:00'),
    ('Garching',                        'Garching, Forschungszentrum',  'U6',  '09:06:00', '09:09:00'),
    ('Garching-Hochbrück',              'Garching',                     'U6',  '09:04:00', '09:06:00'),
    ('Fröttmaning',                     'Garching-Hochbrück',           'U6',  '09:00:00', '09:04:00'),
    ('Garching, Forschungszentrum',     'Technische Universität',       '690', '17:56:00', '17:57:00')
;
