-- a)

select distinct a.Name
from ZehnkampfD as a
where not exists (
	select *
	from ZehmkampfD as a2
	where a2.Name=a.Name
		and exists (
			select *
			from ZehnkampfD as b
			where b.Disziplin=a2.Disziplin
			and b.Name='Bolt'
			and b.Punkte>=a2.Punkte
	)
)

-- b)

with besserAlsBolt(name, disziplin) as (
	select a.name, a.disziplin
	from zehnkampfd a, zehnkampfd b
	where b.name='Bolt'
		and a.disziplin=b.disziplin
		and a.punkte>b.punkte
	),
	disziplinen(anzahl) as (
		select count(distinct disziplin) as anzahl
		from zehnkampfd
	)
	select name
	from besserAlsBolt
	group by name
	having count(*)=(select anzahl from disziplinen)
