(* testing utilities [do not change] *)

exception SyncDeadlocked
module Event = struct
  include Event

  let tsync t e =
    let timer = new_channel () in
    let run_timer () =
      Thread.delay t;
      poll (send timer None)
    in
    let _ = Thread.create run_timer () in
    match (select [wrap e (fun x -> Some x); receive timer]) with
    | Some x -> x
    | None -> raise SyncDeadlocked

  let tselect t es =
    tsync t (choose es)

  let sync e = tsync 2. e
  let select es = tselect 2. es
end

module Thread = struct
  include Thread

  let tc = ref 0

  let create f a =
    tc := !tc + 1;
    create f a
end


(*****************************************************************************)
(*************************** START OF HOMEWORK *******************************)
(*****************************************************************************)
open Thread
open Event

(* 13.4 *)

let par_unary f a=List.map (fun x -> (x, new_channel())) a
	|> List.map (fun x -> let (v,ch)=x in ((create (fun x -> (sync (send ch (f x)))) v),ch))
	|> List.map (fun x -> let (id,ch)=x in sync (receive ch))

let par_binary f a b = List.init (List.length a) (fun n -> ((List.nth a n), (List.nth b n), new_channel()))
	|> List.map (fun x -> let (va,vb,ch)=x in ((create (fun x -> (sync (send ch (f va vb)))) vb),ch))
	|> List.map (fun x -> let (id, ch)=x in sync (receive ch))

(* 13.5 *)

exception OutOfBounds

type 'a req=Get of 'a channel * int | Set of int * 'a | Resize of int * 'a | Size of int channel | Exit
type 'a array='a req channel

module Array=struct
	let size ch=let reply=new_channel() in sync (send ch (Size reply)); sync (receive reply)
	let destroy ch=sync (send ch Exit)
	let set i v ch=if i>=(size ch) then raise OutOfBounds else sync (send ch (Set (i,v)))
	let get i ch=if i>=(size ch) then raise OutOfBounds else let reply=new_channel() in
		sync (send ch (Get (reply,i))); sync (receive reply)
	let resize i v ch=sync (send ch (Resize (i,v)))
	let make s v=let arr=new_channel() in
		let rec serve l=
			match sync(receive arr) with
			| Get (ch, i) -> sync (send ch (List.nth l i)); serve l
			| Set (i, v) -> serve (List.init (List.length l) (fun n -> if n=i then v else List.nth l n))
			| Resize (i, v) -> serve (List.init i (fun n -> if n>=(List.length l) then v else List.nth l n))
			| Size ch -> sync (send ch (List.length l)); serve l
			| Exit -> exit
		in
			create serve (List.init s (fun _ -> v));
			arr
end

(* 13.6 *)
exception InvalidOperation

type 'a docreq=Addacc of string * string | Pub of string * string * string * int channel | View of string * string * int * string channel | Addviewer of string * string * int * string | Chgowner of string * string * int * string
type 'a docserver='a docreq channel

let document_server ()=let server=new_channel() in
		let rec serve t=serve t
	in
		create serve [];
		server

let publish u p doc s=let re=new_channel() in sync (send s (Pub (u,p,doc,re))); sync (receive re)
let change_owner u p id owner s=sync (send s (Chgowner (u,p,id,owner)));s
let view u p id s=let re=new_channel() in sync (send s (View (u,p,id,re))); sync (receive re)
let add_account u p s=sync (send s (Addacc (u,p)));s
let add_viewer u p id viewer s=sync (send s (Addviewer (u,p,id,viewer)))

(*****************************************************************************)
(**************************** END OF HOMEWORK ********************************)
(*****************************************************************************)

(*****************************************************************************)
(* TESTS [do not change] *)
let reset () =
  Thread.tc := 0
let threads_created () =
  !Thread.tc

let d_server () =
  let s = document_server () in
  add_account "user1" "pass1" s;
  add_account "user2" "pass2" s;
  add_account "user3" "pass3" s;
  s

let tests = [
  (* 13.4 *)
  __LINE_OF__ (fun () -> let pinc = par_unary (fun x -> x + 1) in pinc [8;1;1] = [9;2;2] && threads_created () = 3);
  __LINE_OF__ (fun () -> let psof = par_unary string_of_float in psof [7.;1.] = ["7.";"1."] && threads_created () = 2);
  __LINE_OF__ (fun () -> let pmul = par_binary ( * ) in pmul [1;2;3] [5;6;2] = [5;12;6] && threads_created () = 3);
  __LINE_OF__ (fun () -> let pcon = par_binary ( ^ ) in pcon ["th";"";"ver";"nic"] ["is";"is";"y";"e"] = ["this";"is";"very";"nice"] && threads_created () = 4);
  (* 13.5
  NOTE: Array's functions cannot be tested in isolation, so if a test for size fails it may very well be due to a mistake in your implementation of make *)
  __LINE_OF__ (fun () -> let _ = Array.make 3 "abc" in threads_created () = 1);
  __LINE_OF__ (fun () -> let a = Array.make 3 1. in Array.destroy a; threads_created () = 1);
  __LINE_OF__ (fun () -> let a = Array.make 3 0 in Array.size a = 3);
  __LINE_OF__ (fun () -> let a = Array.make 3 'x' in Array.get 0 a = 'x');
  __LINE_OF__ (fun () -> let a = Array.make 3 'x' in try let _ = Array.get 3 a in false with OutOfBounds -> true);
  __LINE_OF__ (fun () -> let a = Array.make 3 0 in Array.set 1 5 a; Array.get 0 a = 0 && Array.get 1 a = 5 && Array.get 2 a = 0 && threads_created () = 1);
  __LINE_OF__ (fun () -> let a = Array.make 3 'x' in try Array.set 3 'u' a; false with OutOfBounds -> true);
  __LINE_OF__ (fun () -> let a = Array.make 3 0 in Array.resize 5 1 a; Array.size a = 5 && Array.get 2 a = 0 && Array.get 3 a = 1 && Array.get 4 a = 1 && threads_created () = 1);
  __LINE_OF__ (fun () -> let a = Array.make 3 0 in Array.resize 1 1 a; Array.size a = 1 && Array.get 0 a = 0 && threads_created () = 1);
  (* 13.6
  NOTE: Document server functions cannot be tested in isolation, so if a test for view fails it may very well be due to a mistake in your implementation of document_server *)
  __LINE_OF__ (fun () -> let _ = document_server () in threads_created () = 1); (* basic thread creation *)
  __LINE_OF__ (fun () -> let s = document_server () in add_account "user1" "pass1" s; true); (* add correct account *)
  __LINE_OF__ (fun () -> let s = d_server () in try add_account "user1" "***" s; false with InvalidOperation -> true); (* account exists already *)
  __LINE_OF__ (fun () -> let s = d_server () in publish "user2" "pass2" "My Document" s <> publish "user1" "pass1" "My Document" s); (* publish document *)
  __LINE_OF__ (fun () -> let s = d_server () in try let _ = publish "user1" "***" "My Document" s in false with InvalidOperation -> true); (* publish incorrect auth *)
  __LINE_OF__ (fun () -> let s = d_server () in try let _ = view "user1" "pass1" 0 s in false with InvalidOperation -> true); (* view invalid document *)
  __LINE_OF__ (fun () -> let s = d_server () in let d = publish "user1" "pass1" "text" s in "text" = view "user1" "pass1" d s); (* view correct *)
  __LINE_OF__ (fun () -> let s = d_server () in let d = publish "user1" "pass1" "text" s in try let _ = view "user2" "pass2" d s in false with InvalidOperation -> true); (* view, no access *)
  __LINE_OF__ (fun () -> let s = d_server () in try add_viewer "user1" "pass1" 0 "user3" s; false with InvalidOperation -> true); (* add viewer invalid document *)
  __LINE_OF__ (fun () -> let s = d_server () in let d = publish "user1" "pass1" "text" s in try add_viewer "user1" "***" d "user3" s; false with InvalidOperation -> (try let _ = view "user3" "pass3" d s in false with InvalidOperation -> true)); (* add viewer invalid auth *)
  __LINE_OF__ (fun () -> let s = d_server () in let d = publish "user2" "pass2" "text" s in add_viewer "user2" "pass2" d "user1" s; view "user1" "pass1" d s = "text"); (* add viewer correct *)
  __LINE_OF__ (fun () -> let s = d_server () in let d = publish "user1" "pass1" "mydoc" s in try change_owner "user1" "***" d "user2" s; false with InvalidOperation -> true); (* change owner invalid auth *)
  __LINE_OF__ (fun () -> let s = d_server () in try change_owner "user1" "pass1" 0 "user3" s; false with InvalidOperation -> true); (* change owner invalid document *)
  __LINE_OF__ (fun () -> let s = d_server () in let d = publish "user1" "pass1" "mydoc" s in try change_owner "user2" "pass2" d "user2" s; false with InvalidOperation -> true); (* change owner, not owner *)
  __LINE_OF__ (fun () -> let s = d_server () in let d = publish "user1" "pass1" "mydoc" s in change_owner "user1" "pass1" d "user3" s; view "user3" "pass3" d s = "mydoc"); (* change owner correct *)
]

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
    reset ();
    let ok = try t () with e -> print_endline (Printexc.to_string e); false in
    if not ok then fail l;
    ok
  in
  let passed = filter (fun x -> x) (map test tests) in
  printf "passed %d/%d tests\n" (length passed) (length tests)





