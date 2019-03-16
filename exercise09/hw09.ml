let enable_additional_knapsack_tests = false
let todo _ = failwith "TODO"

type behavior = Nice | Naughty
type notes = (string * behavior) list
type selection_alg = (string * int * int) list -> int -> string list

exception Invalid_file_format of string

let slurp fn=let file=open_in fn in
	let rec read acc=try read ((input_line file)::acc)
		with End_of_file -> let _=close_in file in acc
	in
	read []

(* 9.3 - 1 *)

let read_notes fn = let process x=match x with
			| n::b::[] ->
				if n="" then raise (Invalid_file_format fn) else
				if b="nice" then (n, Nice) else
				if b="naughty" then (n, Naughty) else
				raise (Invalid_file_format fn)
			| _ -> raise (Invalid_file_format fn)
	in
	slurp fn |>
	List.map (String.split_on_char ':') |>
	List.map process

(* 9.3 - 2 *)
let read_wishlist fn = let process x=match x with
		| p::i::[] -> if p="" then raise (Invalid_file_format fn)
			else let v=try int_of_string i with Failure _ -> raise (Invalid_file_format fn)
			in (p, v)
		| _ -> raise (Invalid_file_format fn)
	in
	slurp fn |>
	List.map (String.split_on_char ':') |>
	List.map process

(* 9.3 - 3 *)
let load_catalogue fn = read_wishlist fn

(* 9.3 - 4 *)
let write_list fn sl = let file=open_out fn in
	let rec dump sl=match sl with
		| s::sr -> let _=output_string file (s^"\n") in dump sr
		| [] -> close_out file
	in
	dump sl

(* 9.3 - 5 *)
let write_letter fn = let file=open_out fn in
	let _=output_string file "Hello, dear naughty child. Yesterday, I watched a little sketch\nwhere a couple of people sit in a room in front of a whiteboard.\nOn the whiteboard one can see the word rapist, and some\nother word covered with paper above it. The person standing\nin front asks: 'What is worse than a rapist?', and then\nreveals the word 'child' above. The audience gasps, and\na man in the background audibly murmurs: 'A child'.\n" in
	let _=close_out file in
	()

(* 9.3 - 6 *)

let toy_weight tn cat=match List.filter (fun x -> let (t, w)=x in t=tn) cat |>
	List.map (fun x -> let (t, w)=x in w) with
	| [] -> None
	| w::ws -> Some w

let add_weights wl cat = List.fold_left (fun acc x -> let (t, i)=x in match toy_weight t cat with
	| Some w -> (t, i, w)::acc
	| None -> acc) [] wl

let run_santas_factory i a= let notes=read_notes "santas_notes.txt" in
	let cat=load_catalogue "toys_catalogue.txt" in
	let handle_child c=match c with
		| (n, Naughty) -> write_letter (n^"_letter.txt")
		| (n, Nice) -> let wl=read_wishlist (n^"_wishlist.txt") in
			write_list (n^"_presents.txt") (a (add_weights wl cat) i)
	in
	List.map handle_child notes

(* 9.3 - 7 *)

let rec m p w s=match p with
	| [] -> (0, s)
	| (ni, vi, wi)::ps -> if wi>w then m ps w s else
		let (fw, fp)=m ps w s and (aw, ap)=m ps (w-wi) (ni::s) in
		if fw>(aw+vi) then (fw, fp) else (aw+vi, ap)

let knapsack pres w=let (w, sp)=m pres w [] in sp

(*****************************************************************************)
(**************************** END OF HOMEWORK ********************************)
(*****************************************************************************)
(* example inputs, you may use them to test your implementations,
   but [do not change] *)
let a933_ex1 = ["penguin doll",1; "ocaml book",2; "time machine",53; "bike",7; "barbie's dream house",5;
  "guitar",6; "colorful pencils",2; "socks",1; "shawl",2; "karaoke machine",13; "superman action doll set",3;
  "guinea pig",3; "horse",10; "unicorn",8; "sand toys",4; "soccer shoes",3]

let a937_ex0 = [("0",24,7);("1",50,6);("2",37,6);("3",19,1);("4",32,2);("5",71,8);("6",54,2);("7",87,8);("8",9,6);("9",80,9);("10",21,9);("11",93,0);("12",5,4);("13",97,5);("14",34,8);("15",42,7);("16",11,1);("17",71,4);("18",1,8);("19",39,0)]
let a937_ex0_result = ["11";"19";"3";"6";"17";"13";"7"]
let a937_ex1 = [("0",16,6);("1",65,4);("2",26,0);("3",67,4);("4",93,9);("5",85,0);("6",2,6);("7",6,1);("8",76,4);("9",30,1);("10",50,1);("11",50,4);("12",53,5);("13",19,9);("14",1,3)]
let a937_ex1_result = ["2";"5";"7";"9";"10";"1";"3";"8";"12"]
let a937_ex2 = [("0",30,1);("1",31,8);("2",64,8);("3",54,6);("4",71,2);("5",44,9);("6",22,5);("7",67,5);("8",55,2);("9",24,8);("10",80,7);("11",40,5);("12",68,3);("13",90,7);("14",71,1);("15",43,3);("16",62,5);("17",40,8);("18",54,2);("19",27,1)]
let a937_ex2_result = ["0";"14";"19";"4";"8";"18";"12";"15";"7"]
let a937_ex3 = [("0",59,8);("1",45,7);("2",29,9);("3",43,3);("4",64,1);("5",71,2);("6",8,8);("7",43,4);("8",4,1);("9",22,8);("10",52,3);("11",60,1);("12",69,3);("13",12,2);("14",99,1);("15",94,3);("16",76,2);("17",68,8);("18",83,9);("19",29,3);("20",56,6);("21",21,5);("22",53,2);("23",86,1);("24",81,4)]
let a937_ex3_result = ["4";"11";"14";"23";"5";"16";"22";"12";"15";"24"]


(*****************************************************************************)
(* TESTS [do not change] *)
let (=^) a b =
    (List.sort compare a) = (List.sort compare b)
let (=|) a b =
    let a = List.sort_uniq (fun x y -> compare (fst x) (fst y)) a in
    let b = List.sort_uniq (fun x y -> compare (fst x) (fst y)) b in
    a = b
let check_throws e f =
  try f (); false with e' -> e' = e

let check_file filename content =
  let file = open_in filename in
  let rec read acc =
    try
      read ((input_line file)::acc)
    with End_of_file -> acc
  in
  let c = read [] in
  close_in file;
  (List.sort_uniq compare c) = (List.sort_uniq compare content)

let check_letter filename =
  let file = open_in filename in
  let rec read () =
    try
      let line = input_line file in
      if line <> "" then true else
      read ()
    with End_of_file -> false
  in
  let r = read () in
  close_in file;
  r

let raise' = function Failure f ->
  Printf.printf "TEST FAILURE: %s\n" f;
  raise (Failure f)
  | e -> raise e

let check_run_santas_factory () =
  let test_selection_alg wishes capacity =
    if capacity <> 13 then raise' (Failure "wrong capacity passed to selection_alg");
    (match List.find_opt (fun (t,_,_) -> t = "ocaml book") wishes with
    | None -> raise' (Failure "wrong list passed to selection_alg")
    | Some (_,_,w) -> if w <> 2 then raise' (Failure "wrong list passed to selection_alg"));
    match List.sort (fun (_,i,_) (_,j,_) -> compare j i) wishes with
    | (w1,_,_)::(w2,_,_)::_ -> [w1;w2]
    | _ -> raise' (Failure "wrong list passed to selection_alg")
  in
  ignore(run_santas_factory 13 test_selection_alg);
  if not (check_letter "marta_letter.txt") then raise (Failure "no correct letter produced for marta");
  if not (check_letter "bruno_letter.txt") then raise (Failure "no correct letter produced for bruno");
  if not (check_file "frida_presents.txt" ["colorful pencils";"ocaml book"]) then raise (Failure "no correct present list produced for frida");
  if not (check_file "tommy_presents.txt" ["sand toys";"superman action doll set"]) then raise (Failure "no correct present list produced for tommy");
  if not (check_file "caren_presents.txt" ["penguin doll";"unicorn"]) then raise (Failure "no correct present list produced for caren");
  true

let ks_a937_ex1_rt : float option ref = ref None
let ks_a937_ex2_rt : float option ref = ref None
let ks_a937_ex3_rt : float option ref = ref None
let check_is_knapsack_efficient () =
  match !ks_a937_ex1_rt, !ks_a937_ex2_rt, !ks_a937_ex3_rt with
  | Some a, Some b, Some c -> (* Printf.printf "a = %f, b = %f, c = %f, quotients: %.2f | %.2f\n" a b c (b /. a) (c /. b); *) (b /. a) <= 5. && (c /. b) <= 5.
  | _ -> false

let measure_ks_runtime alg input size =
    let start = Sys.time () in
    let result = alg input size in
    result, Sys.time () -. start


let tests = [
  (* tests for 9.3 - 1 *)
  __LINE_OF__ (fun () -> (read_notes "examples/santas_notes.txt") =| ["tommy",Nice;"bruno",Naughty;"frida",Nice;"caren",Nice;"marta",Naughty]);
  __LINE_OF__ (fun () -> let fn = "examples/santas_notes_broken1.txt" in check_throws (Invalid_file_format fn) (fun () -> read_notes fn));
  __LINE_OF__ (fun () -> let fn = "examples/santas_notes_broken2.txt" in check_throws (Invalid_file_format fn) (fun () -> read_notes fn));
  (* tests for 9.3 - 2 *)
  __LINE_OF__ (fun () -> (read_wishlist "examples/frida_wishlist.txt") =| ["ocaml book",10;"horse",3;"colorful pencils",12]);
  __LINE_OF__ (fun () -> let fn = "examples/wishlist_broken1.txt" in check_throws (Invalid_file_format fn) (fun () -> read_wishlist fn));
  __LINE_OF__ (fun () -> let fn = "examples/wishlist_broken2.txt" in check_throws (Invalid_file_format fn) (fun () -> read_wishlist fn));
  __LINE_OF__ (fun () -> let fn = "examples/wishlist_broken3.txt" in check_throws (Invalid_file_format fn) (fun () -> read_wishlist fn));
  (* tests for 9.3 - 3 *)
  __LINE_OF__ (fun () -> (load_catalogue "examples/toys_catalogue.txt") =| a933_ex1);
  __LINE_OF__ (fun () -> let fn = "examples/toys_catalogue_broken1.txt" in check_throws (Invalid_file_format fn) (fun () -> load_catalogue fn));
  __LINE_OF__ (fun () -> let fn = "examples/toys_catalogue_broken2.txt" in check_throws (Invalid_file_format fn) (fun () -> load_catalogue fn));
  __LINE_OF__ (fun () -> let fn = "examples/toys_catalogue_broken3.txt" in check_throws (Invalid_file_format fn) (fun () -> load_catalogue fn));
  (* tests for 9.3 - 4 *)
  __LINE_OF__ (fun () -> let l = ["socks";"colorful pencils";"horse"] in let fn = "examples/testout_list1.txt" in write_list fn l; check_file fn l);
  (* tests for 9.3 - 5 *)
  __LINE_OF__ (fun () -> let fn = "examples/testout_letter1.txt" in write_letter fn; check_letter fn);
  (* tests for 9.3 - 6 *)
  __LINE_OF__ (fun () -> check_run_santas_factory ());
  (* tests for 9.3 - 7 *)
  __LINE_OF__ (fun () -> (knapsack ["a",5,4; "b",2,2; "b",2,2; "d",4,5; "b",2,2; "e",8,2] 10) =^ ["a";"b";"b";"e"]);
  __LINE_OF__ (fun () -> (knapsack ["a",5,4; "a",5,4; "c",11,6; "d",4,5; "e",8,2; "a",5,4] 10) =^ ["c";"e"]);
] @ if enable_additional_knapsack_tests then [
  (* additional tests *)
  __LINE_OF__ (fun () -> (knapsack ["a",100,10; "b",81,9; "c",81,9] 18) =^ ["b";"c"]); (* correctness check 0a *)
  __LINE_OF__ (fun () -> (knapsack a937_ex0 20) =^ a937_ex0_result); (* correctness check 0b *)
  __LINE_OF__ (fun () -> let result, rt = measure_ks_runtime knapsack a937_ex1 20 in ks_a937_ex1_rt := Some rt; result =^ a937_ex1_result); (* correctness check 1 *)
  __LINE_OF__ (fun () -> let result, rt = measure_ks_runtime knapsack a937_ex2 20 in ks_a937_ex2_rt := Some rt; result =^ a937_ex2_result); (* correctness check 2 *)
  __LINE_OF__ (fun () -> let result, rt = measure_ks_runtime knapsack a937_ex3 20 in ks_a937_ex3_rt := Some rt; result =^ a937_ex3_result); (* correctness check 3 *)
  __LINE_OF__ (fun () -> check_is_knapsack_efficient ());
] else []

let () =
  let rec input_lines ch =
    (try Some (input_line ch) with _ -> None) (* catch stupid EOF exception *)
    |> function Some line -> line :: input_lines ch | None -> []
  in
  let lines = input_lines (open_in __FILE__) in
  let open List in
  let open Printf in
  let fail l =
    let line = nth lines (l-1) in
    let test = String.sub line 25 (String.length line - 27) in
    printf "test \027[31;m%s\027[0;m (line %d) failed!\n" test l;
  in
  let test (l, t) =
    let ok = try t () with e -> print_endline (Printexc.to_string e); false in
    if not ok then fail l;
    ok
  in
  let passed = filter (fun x -> x) (map test tests) in
  printf "passed %d/%d tests\n" (length passed) (length tests)

