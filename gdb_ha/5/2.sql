-- a)

select * from Fahrplan where Von like '%Garching%'

-- b)

select *, (case when Linie like "U%" then "U-Bahn"
		when Linie like "S%" then "S-Bahn"
		else "Bus/Tram" end)
	from Fahrplan

-- c)

select * from Fahrplan where timediff(current_time, Fahrplan.Abfahrt)>0 and Fahrplan.Von="Garching, Forschungszentrum" sort by extract(Minute from Abfahrt);

-- d)

select f1.Abfahrt, f2.Ankunft from Fahrplan f1, Fahrplan f2 where
extract(minute from f2.Ankunft)-extract(minute from f1.Abfahrt)>=3 and
extract(minute from f2.Ankunft)-extract(minute from f1.Abfahrt)<=5;
