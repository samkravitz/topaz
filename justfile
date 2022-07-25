debug:
	make debug -j4

release:
	make release -j4

test: release
	rspec spec/run.rb
