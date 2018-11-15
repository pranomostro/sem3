#!/bin/bash

echo "1. Installing system requirements"
sudo apt-get update -qq
sudo apt install git m4 make curl
read -p "Step 1 done. Press enter to continue."

echo "2. Installing opam: confirm prompts with y to adjust your shell config!"
sh <(curl -sL https://raw.githubusercontent.com/ocaml/opam/master/shell/install.sh)
read -p "Step 2 done. Press enter to continue."

echo "3. Installing opam packages"
opam update
opam install utop merlin ocp-indent
read -p "Step 3 done. Press enter to continue."

echo "4. Check"
opam --version
ocaml --version
echo 'print_endline "hello";;' | ocaml
