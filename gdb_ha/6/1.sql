-- a)

with recursive fahrplan_rec as (
	select von, nach, abfahrt, ankunft from fahrplan
	union all
	select fr.von, f.nach. fr.abfahrt, f.ankunft
	from fahrplan_rec fr, fahrplan f
	where
		fr.nach=f.von and
		fr.ankunft<=f.abfahrt and
		fr.von!=f.nach)
select * from fahrplan_rec;

-- b)

with recursive fahrplan_rec_linie as (
	select von,
		nach,
		abfahrt,
		ankunft,
		ankunft-abfahrt as fahrtzeit,
		cast('00:00:00' as interval) as wartezeit
	from fahrplan
	union all
	select
		fr.von,
		f.nach,
		fr.abfahrt,
		f.ankunft,
		fr.fahrtzeit+(f.ankunft-fr.abfahrt),
		fr.wartezeit+(f.abfahrt-fr.ankunft)
	from fahrplan_rec_linie fr, fahrplan f
	where
		fr.nach=f.von and
		fr.ankunft<=f.abfahrt and
		fr.von!=f.nach
),
fahrplan_rec as (
	select
		von,
		nach,
		abfahrt,
		ankunft,
		fahrtzeit,
		wartezeit,
		fahrtzeit+wartezeit as reisezeit
	from fahrplan_rec_linie
)
select * from fahrplan_rec;

-- c)

with recursive fahrplan_rec_linie as (
	select von,
		nach,
		abfahrt,
		ankunft,
		linie as aktuelle_linie,
		0 as umstiege,
		ankunft-abfahrt as fahrtzeit,
		cast('00:00:00' as interval) as wartezeit
	from fahrplan
	union all
	select
		fr.von,
		f.nach,
		fr.abfahrt,
		f.ankunft,
		f.linie,
		fr.umstiege+case
			when
				f.linie!=fr.aktuelle_linie or
				f.abfahrt>fr.ankunft
			then 1
			else 0
		end,
		fr.fahrtzeit+(f.ankunft-fr.abfahrt),
		fr.wartezeit+(f.abfahrt-fr.ankunft)
	from fahrplan_rec_linie fr, fahrplan f
	where
		fr.nach=f.von and
		fr.ankunft<=f.abfahrt and
		fr.von!=f.nach
),
fahrplan_rec as (
	select
		von,
		nach,
		abfahrt,
		ankunft,
		umstiege,
		fahrtzeit,
		wartezeit,
		fahrtzeit+wartezeit as reisezeit
	from fahrplan_rec_linie
)
select * from fahrplan_rec;

-- d)

with recursive fahrplan_rec_linie as (
	select von,
		nach,
		abfahrt,
		ankunft,
		linie as aktuelle_linie,
		0 as umstiege,
		ankunft-abfahrt as fahrtzeit,
		cast('00:00:00' as interval) as wartezeit
	from fahrplan
	union all
	select
		fr.von,
		f.nach,
		fr.abfahrt,
		f.ankunft,
		f.linie,
		fr.umstiege+case
			when
				f.linie!=fr.aktuelle_linie or
				f.abfahrt>fr.ankunft
			then 1
			else 0
		end,
		fr.fahrtzeit+(f.ankunft-fr.abfahrt),
		fr.wartezeit+(f.abfahrt-fr.ankunft)
	from fahrplan_rec_linie fr, fahrplan f
	where
		fr.nach=f.von and
		fr.ankunft<=f.abfahrt and
		fr.von!=f.nach
),
fahrplan_rec as (
	select
		von,
		nach,
		abfahrt,
		ankunft,
		umstiege,
		fahrtzeit,
		wartezeit,
		fahrtzeit+wartezeit as reisezeit
	from fahrplan_rec_linie
)
select * from fahrplan_rec as fpr where
	fpr.ankunft<=TIME '10:30:00' and
	fpr.nach='Garching, Forschungszentrum' and
	fpr.von='Fröttmanning' and
	not exists(
		select * from fahrplan_rec fpr2 wher
		fpr2.von=fpr.von and
		fpr2.nach=fpr.nach and
		fpr2.ankunft<=TIME '10:30:00' and
		fprg2.abfahrt>fpr.abfahrt and
		fpr2.reisezeit<fpr.reisezeit and
		fpr2.umstiege<fpr.umstiege);
