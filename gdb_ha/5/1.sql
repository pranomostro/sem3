select Name from Studenten s, (select MatrNr from hören having count(MatrNr)>=3) v where s.MatrNr=v.MatrNr;
