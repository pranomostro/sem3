-- a)

(select distinct count(A, B) from R) = (select distinct count(*) from R)

-- b)

(select distinct count(D, E) from R) = (select distinct count(B) from R)
