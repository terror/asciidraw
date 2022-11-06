set dotenv-load

export EDITOR := 'vim'

alias r := run
alias c := clean

default:
  just --list

run:
	gcc -o asciidraw asciidraw.c
	./asciidraw
	@just clean

clean:
	rm -rf a.out
