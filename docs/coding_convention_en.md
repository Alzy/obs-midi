---
layout: page
title: Coding
permalink: /coding/
---
This agreement describes the principles of design and version control of the code in the development of the project.
The agreement is being developed, finalized and discussed.

1. Naming classes, variables, functions, signals, and slots
	Classes, variables and functions are named in the snake_case style: lowercase (small) letters, words are separated by an underscore. An underscore at the beginning and end of a word is not welcome.
	Variables-class fields are called nouns or phrases (no more than 3-4 words): counter, start_time. Boolean variables can have a name derived from an adjective: saved, edited. The prefix that indicates the type or nature of the variables are not welcome.
	Local variables can have a short name that makes sense in the scope context: sent, i, it. These variables should only be used within blocks of small size (3-5 lines).
	Classes are named according to the task they solve: visualiser, main_window, data_handler.
	Functions are named according to the action being performed: set_value (), get_value(), process_event ().
	Signals are named according to the event they reflect. Must have the signal_ prefix: signal_time_changed();
	Slots are named according to the action that they perform, or the event that they react to. Must have the slot_ prefix: slot_time_changed (), slot_on_time_changed (), slot_set_time();
	
2.Spaces or tabs
	Tab characters are used to organize horizontal margins. The width of a single tab character is 4 spaces.

3. The organization of classes and file naming
	Each class must be located in a separate file. Exceptions may be: classes nested in classes, groups of small classes.
	
4. Brackets
	The opening curly brace is placed on a new line, with the horizontal indent of the block header:
	if (/**/)
	{
		//
	}
	The closing curly brace is placed with the horizontal indentation of the corresponding opening curly brace on a separate line.
	The bodies of conditional statements and loop statements are always framed by curly brackets.
	In the case of dividing an expression framed by parentheses into several lines, the closing curly brace must be placed under the opening one.
	Parentheses must be separated from operators by spaces; no space is placed between the parenthesis and the operand: if (a + b){}; get_value(); (in the latter case, the function name is the operand).
	
5. The code width
	is 160-180 characters (including comments).
	
7. How to work with branches
	Branches are named according to their content and are prefixed according to the card number in the project management system: is231_optimization.
	In general, branches should be merged into master. In exceptional cases, it is allowed to merge other branches with each other (this may make it difficult to merge into master).

8. Included files
	In header files, the number of files to include should be minimal, only those files that contain definitions of the types and functions used in this file are included. Everything you need for a cpp file is included directly in it.
	The order of the included files: standard C library, standard C++ library, system header files, third-party library files, our library files, project files:
	#include <stdio.h> // standard C library

	#include <vector> // standard C++ library

	#include <windows.h> // system header files

	#include <QVBoxLayout> // third-party library files

	#include <lib.h> // files in our library

	#include "parameter.h" // project files

	It is advisable to separate the listed sections with an empty line.
	
9. The use of namespaces
	In header files, the use of using namespace outside of classes and functions is prohibited, and the namespace is explicitly specified.
	using namespace should be written in cpp files.

10. Order of fields in the class
	public
	public slots
	signals
	protected
	private
	private slots
	
	Inside the blocks, typedefs are first placed, then variables, then methods. There should be no variables in public blocks.
	Among the methods, first there are constructors, then a destructor, then other methods, then operators.

11. Naming widget variables
	Variable pointers to widget objects must have a prefix corresponding to this type of widget: button_ok, layout_main, label_time_to_wait.
	
12. Global variables
	Forbidden.

13. Names of local variables
	Should always be different from the field names of the class

14. Constant functions
	All functions that do not change the class fields should be marked with the const keyword.

15. The input parameters of the functions
	Wherever possible, the input parameters must be constant references (the exception is the parameters of primitive types, since copying them is the same as copying references). For example:
		void set_smth(const QString& value);

16. The return values of functions
	Everything described below is meaningless for primitive types, because copying them takes the same time as copying a reference.

	Everywhere, where the fields of the class (not the local variables of the function!!!) provided only for reading (for example, geters), the return value should be wrapped in a constant reference. If there is a need to write the return value to a non-const variable, then in addition to the reference getter, you need to create a regular getter, in this case the compiler will choose the best option. For example:
		const QString& get_smth() const;
		QString get_smth() const;

	If you create a new object inside a function and return it by value, you should use std::move(). For example:
		QString do_smth() const
		{
			QString output = "";
			output = "bla-bla-bla";
			return std::move(output);
		}
