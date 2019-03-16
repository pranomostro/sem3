
module type Set = sig
  type t
  val to_string : t -> string
end

module type Map = sig
  type key
  type value
  type t

  val empty : t
  val set : key -> value -> t -> t
  val get : key -> t -> value
  val get_opt : key -> t -> value option
  (* val clear : key -> t -> t *)
  val to_string : t -> string
end

module type OrderedSet=sig
end

let join l s=List.fold_right (fun x r -> if r="" then x else x^s^r) l ""

module BTreeMap (K : OrderedSet) (V : Set) : Map = struct
	type key=K.t
	type value=V.t
	type f=Empty | Node of key * value * t * t
	let to_string t=let rec flatten t=
		match t with
		| Empty -> []
		| Node(k, v, t1, t2) -> (k, v)::((flatten t1)@(flatten t2))
	in let sft=List.sort (fun x y -> let (k1,v1)=x and (k2, v2) in compare k1 k2) (flatten t)
	in "{"^(join (List.map (fun x -> let (k, v)=x in (to_string k)^" -> "^(to_string v)) ", "))^"}"
end
