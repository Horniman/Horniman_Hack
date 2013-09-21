#Integration
#-----------
#
#The source includes a ``Makefile`` that checks the following types of conformance. Please run ``make`` prior to any ``git push``.
#

.PHONY:all htmllint lint code_coverage doc
.SECONDARY:

all: code_coverage htmllint doc lint

#Code style
#^^^^^^^^^^
#
#We use ``pylint`` to check our prefered code-style for all the Django Python code. 
#We mainly try and stick to :pep:`8` formating, with some allowencies for Django and expediance. 
#Comments are forced but can be empty (attesting that the developer felt they would be redundent; 
#pending comments are left out so the lint check fails).
#
#`Code style report <../../pylint.html>`_
#
lint:
	-(cd ..;bin/pylint --rcfile=horni/.pylintrc -f colorized -r n horni)

htmllint:
	-(cd ..;bin/pylint --rcfile=horni/.pylintrc -f html horni) > static/pylint.html

#Tests
#^^^^^
#
#We use Djang's testing framework to write tests for all our Djano Apps.
#
#.. todo::
#
#    Proper unit tests for the Javascript are being worked on.
#
test:
	./manage.py test

#Coverage
#^^^^^^^^
#
#We use :py:mod:`air.harness.management.commands.code_coverage` to check the test coverage of our Django Apps.
#
#`Code coverage report <../../coverage_report/>`_
#
code_coverage:
	./manage.py code_coverage

#Documentation
#^^^^^^^^^^^^^
#
#We use ``Sphinx`` to generate the master documentation for the project. 
#
doc:
	make -C doc html
