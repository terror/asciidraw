set dotenv-load

export EDITOR := 'vim'

alias r := run
alias c := compile

default:
  just --list

clean:
  rm -rf a.out

compile:
  gcc -o asciidraw asciidraw.c

forbid:
  ./bin/forbid

run:
  @just compile
  ./asciidraw
  @just clean
