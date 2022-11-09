set dotenv-load

export EDITOR := 'vim'

alias r := run
alias c := compile

default:
  just --list

compile:
  gcc -o asciidraw asciidraw.c

run:
  @just compile
  ./asciidraw
  @just clean

clean:
  rm -rf a.out
