with vmh as (select
	v.Titel, s.Semester, count(s.Semester) as hoerer
from
	Vorlesungen v, hoeren h, Studenten s
where
	v.VorlNr=h.VorlNr and h.MatrNr=s.MatrNr
group by
	s.Semester, v.Titel
order by
	v.Titel),
modus as (select * from vmh v1 where v1.hoerer>=(select v2.hoerer from vmh v2 where v1.Titel=v2.Titel))
select v3.* from vmh v3, modus where v3.Semester<modus.Semester and v3.Titel=modus.Titel
