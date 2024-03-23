#!/bin/sh
# shellcheck shell=sh


## ======================================================================== ##
## Copyright 2020 Intel Corporation                                         ##
##                                                                          ##
## Licensed under the Apache License, Version 2.0 (the "License");          ##
## you may not use this file except in compliance with the License.         ##
## You may obtain a copy of the License at                                  ##
##                                                                          ##
##     http://www.apache.org/licenses/LICENSE-2.0                           ##
##                                                                          ##
## Unless required by applicable law or agreed to in writing, software      ##
## distributed under the License is distributed on an "AS IS" BASIS,        ##
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. ##
## See the License for the specific language governing permissions and      ##
## limitations under the License.                                           ##
## ======================================================================== ##

# This script sets up the runtime environment
# ############################################################################
#
# Get absolute path to script, when sourced from bash, zsh and ksh shells.
# Uses `readlink` to remove links and `pwd -P` to turn into an absolute path.
# Derived from a function created by Sergey Shkrunin for use by the analyzers.
# Converted into a POSIX-compliant function.

# Usage:
#   script_dir=$(get_script_path "$script_rel_path")
#
# Inputs:
#   script/relative/pathname/scriptname
#
# Outputs:
#   /script/absolute/pathname

# executing function in a *subshell* to localize vars and effects on `cd`
get_script_path() (
  script="$1"
  while [ -L "$script" ] ; do
    # combining next two lines fails in zsh shell
    script_dir=$(command dirname -- "$script")
    script_dir=$(cd "$script_dir" && command pwd -P)
    script="$(readlink "$script")"
    case $script in
      (/*) ;;
       (*) script="$script_dir/$script" ;;
    esac
  done
  # combining next two lines fails in zsh shell
  script_dir=$(command dirname -- "$script")
  script_dir=$(cd "$script_dir" && command pwd -P)
  echo "$script_dir"
)


# ############################################################################

# Check to insure that this script is being sourced, not executed.
# see https://stackoverflow.com/a/38128348/2914328

# This script is designed to be POSIX compatible, there are a few lines in the
# code block below that contain elements that are specific to a shell. The
# shell-specific elements are needed to identify the sourcing shell.

usage() {
  printf "%s\n"   "ERROR: This script must be sourced."
  printf "%s\n"   "Usage: source $1"
  exit 255
}

vars_script_name=""
# ${var:-} needed to pass "set -eu" checks
# see https://unix.stackexchange.com/a/381465/103967
# see https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_06_02
if [ -n "${ZSH_VERSION:-}" ] ; then     # only executed in zsh
  # echo "   ZSH version = \"$ZSH_VERSION\""
  case $ZSH_EVAL_CONTEXT in (*:file*) vars_script_name="${(%):-%x}" ;; esac ;
  if [ -n "$ZSH_EVAL_CONTEXT" ] && [ "" = "$vars_script_name" ] ; then
    usage "${(%):-%x}" ;
  fi
elif [ -n "${KSH_VERSION:-}" ] ; then   # only executed in ksh or mksh or lksh
  # echo "   KSH version = \"$KSH_VERSION\""
  if [ "$(set | grep KSH_VERSION)" = "KSH_VERSION=.sh.version" ] ; then # ksh
    if [ "$(cd "$(dirname -- "$0")" && pwd -P)/$(basename -- "$0")" \
      != "$(cd "$(dirname -- "${.sh.file}")" && pwd -P)/$(basename -- "${.sh.file}")" ] ; then
      vars_script_name="${.sh.file}" || usage "$0" ;
    fi
  else # mksh or lksh detected (also check for [lm]ksh renamed as ksh)
    _lmksh="$(basename -- "$0")" ;
    if [ "mksh" = "$_lmksh" ] || [ "lksh" = "$_lmksh" ] || [ "ksh" = "$_lmksh" ] ; then
      # force [lm]ksh to issue error msg; contains this script's rel/path/filename
      vars_script_name="$( (echo "${.sh.file}") 2>&1 )" || : ;
      vars_script_name="$(expr "$vars_script_name" : '^.*ksh: \(.*\)\[[0-9]*\]:')" ;
    fi
  fi
elif [ -n "${BASH_VERSION:-}" ] ; then  # only executed in bash
  # echo "   BASH version = \"$BASH_VERSION\""
  # shellcheck disable=2128,2015
  (return 0 2>/dev/null) && vars_script_name="${BASH_SOURCE}" || usage "${BASH_SOURCE}"
else
  case ${0##*/} in (sh|dash) vars_script_name="" ;; esac
  # echo "   Shell version = \"unknown\""
fi

if [ "" = "$vars_script_name" ] ; then
  >&2 echo ":: ERROR: Unable to proceed: no support for sourcing from '[dash|sh]' shell." ;
  >&2 echo "   This script must be sourced. Did you execute or source this script?" ;
  >&2 echo "   Can be caused by sourcing from inside a \"shebang-less\" script." ;
  >&2 echo "   Can also be caused by sourcing from ZSH version 4.x or older." ;
  return 1 2>/dev/null || exit 1
fi



# ############################################################################


# Remeber current directory
ONEAPI_RENDER_CURRENTDIR=$(pwd)
ONEAPI_RENDER_RKUTIL_SCRIPT_PATH=$(get_script_path "$vars_script_name")


ONEAPI_RENDER_RKUTIL_BIN=$ONEAPI_RENDER_RKUTIL_SCRIPT_PATH/../bin
ONEAPI_RENDER_RKUTIL_LIB=$ONEAPI_RENDER_RKUTIL_SCRIPT_PATH/../lib


if [ -d "$ONEAPI_RENDER_RKUTIL_LIB" ]; then

    # go to lib dir in order to acquire full path without relative dot dot slash characters
    cd "$ONEAPI_RENDER_RKUTIL_LIB" || return
    ONEAPI_RENDER_RKUTIL_LIB=$(pwd)

    export LD_LIBRARY_PATH=$ONEAPI_RENDER_RKUTIL_LIB:$LD_LIBRARY_PATH
fi


if [ -d "$ONEAPI_RENDER_RKUTIL_BIN" ]; then 

    # go to bin dir in order to acquire full path without relative dot dot slash characters
    cd "$ONEAPI_RENDER_RKUTIL_BIN" || return
    ONEAPI_RENDER_RKUTIL_BIN=$(pwd)

    export PATH=$ONEAPI_RENDER_RKUTIL_BIN:$PATH
fi
 
#go back to starting dir
cd "$ONEAPI_RENDER_CURRENTDIR" || return

