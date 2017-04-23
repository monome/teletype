Contributing
==================================

Things you will need
--------------------

* a [git](https://git-scm.com/) client (used for source version control).
* a [GitHub](https://github.com/) account (to contribute changes).
* a C compiler and `make` to build sources.
* the [libavr32](https://github.com/monome/libavr32) toolchain (if you want to build the firmware).
* an ssh client (used to authenticate with GitHub).

Getting the code and configuring your environment
-------------------------------------------------

* Fork `https://github.com/monome/teletype` into your own GitHub account (more on forking [here](https://help.github.com/articles/fork-a-repo/).)
* If you haven't configured your machine with an SSH key that's known to github then follow these [directions](https://help.github.com/articles/generating-ssh-keys/).
* Create a directory for your sources.
* Clone the repo locally with the `--recursive` flag (to get the `libavr32` submodule):  `git clone --recursive https://github.com/<my github acct>/teletype.git`
* Run `git remote add upstream git@github.com:monome/teletype.git` (So that you fetch from the master repository, not your clone, when running `git fetch` et al.)

Building and running the code
-----------------------------

To build all:

```
cd src/
make
```

To just build (just) the simulator (and non-firmware teletype library):

```
cd src/sim
make
```

To run the simulator:

```
cd src/sim
./tt
```

Contributing code
-----------------

We gladly accept contributions via GitHub pull requests.

To start working on a patch:

* `git fetch upstream`
* `git checkout upstream/master -b name_of_your_branch`
* Hack away.
* `git commit -a -m "<your brief but informative commit message>"`
* `git push origin name_of_your_branch`

To send us a pull request:

* `git pull-request` (if you are using [Hub](http://github.com/github/hub/)) or
go to `https://github.com/monome/teletype` and click the
"Compare & pull request" button.

