type student = {
	first_name : string;
	last_name : string;
	id : int;
	semester : int;
	grades : (int * float) list
}
type database = student list

let load_db fn=let file=open_in fn in
	let rec read_grades count grades=
		if count=0 then grades else
		let g=match String.split_on_char ';' (input_line file) with
			| c::g::[] -> (int_of_string c, float_of_string g)
			| _ -> failwith "Incorrect line format"
		in read_grades (count-1) (g::grades)
	in
	let rec read_students db=
		try
		let s=match String.split_on_char ';' (input_line file) with
			| [first_name;last_name;sid;sem;gc] -> let id=int_of_string sid and
				semester=int_of_string sem and grc=int_of_string gc in
			{first_name;last_name;id;semester;grades=(read_grades grc [])}
			| _ -> failwith "Incorrect line format"
		in read_students (s::db)
		with End_of_file -> close_in file; db
	in
	read_students []
