-- a)

select s.Name from
(select MatrNr, count(MatrNr) as VorlGehoert from hoeren group by MatrNr) g,
Studenten s where g.VorlGehoert>=3 and g.MatrNr=s.MatrNr;

-- b)

select VorlNr, Titel from Vorlesungen v,
(select Vorgaenger, count(Vorgaenger) as ZahlVorg from voraussetzen group by Vorgaenger) a
where SWS>=4 and v.VorlNr=a.Vorgaenger and a.ZahlVorg>=2;

-- c)

select avg(semester) from
(select distinct s.Semester, s.Name from Studenten s, hoeren h, Vorlesungen v, Professoren p
where s.MatrNr=h.MatrNr and h.VorlNr=v.VorlNr and v.gelesenVon=p.PersNr and p.Name='Sokrates');

-- d)

select h.MatrNr, s.MatrNr, Name from hoeren h right outer join studenten s on h.MatrNr=s.MatrNr;
