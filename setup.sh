#!/bin/bash

(

set -euo pipefail

cd $(dirname ${BASH_SOURCE[0]})

PKGDIR="$(readlink -f .)"
declare -i doPrintEnv=0
declare -i doPrintEnvInstr=0
declare -a setupArgs=()

for farg in "$@"; do
  fargl="$(echo $farg | awk '{print tolower($0)}')"
  if [[ "$fargl" == "env" ]]; then
    doPrintEnv=1
  elif [[ "$fargl" == "envinstr" ]]; then
    doPrintEnvInstr=1
  else
    setupArgs+=( "$farg" ) 
  fi
done
declare -i nSetupArgs
nSetupArgs=${#setupArgs[@]}


printenv() {
  pythonappend="${PKGDIR}/python"
  end=""
  if [[ ! -z "${PYTHONPATH+x}" ]]; then
    end=":${PYTHONPATH}"
  fi
  if [[ "${end}" != *"$pythonappend"* ]]; then
    echo "export PYTHONPATH=${pythonappend}${end}"
    export PYTHONPATH=${pythonappend}${end}
  fi

  libappend="${PKGDIR}/lib"
  end=""
  if [[ ! -z "${LD_LIBRARY_PATH+x}" ]]; then
    end=":${LD_LIBRARY_PATH}"
  fi
  if [[ "${end}" != *"$libappend"* ]]; then
    echo "export LD_LIBRARY_PATH=${libappend}${end}"
    export LD_LIBRARY_PATH=${libappend}${end}
  fi

  rincpathappend="${PKGDIR}/interface"
  end=""
  if [[ ! -z "${ROOT_INCLUDE_PATH+x}" ]]; then
    end=":${ROOT_INCLUDE_PATH}"
  fi
  if [[ "${end}" != *"$rincpathappend"* ]]; then
    echo "export ROOT_INCLUDE_PATH=${rincpathappend}${end}"
    export ROOT_INCLUDE_PATH=${rincpathappend}${end}
  fi

  pathappend="${PKGDIR}/executables"
  end=""
  if [[ ! -z "${PATH+x}" ]]; then
    end=":${PATH}"
  fi
  if [[ "${end}" != *"$pathappend"* ]]; then
    echo "export PATH=${pathappend}${end}"
    export PATH=${pathappend}${end}
  fi
}
doenv() {
  pythonappend="${PKGDIR}/python"
  end=""
  if [[ ! -z "${PYTHONPATH+x}" ]]; then
    end=":${PYTHONPATH}"
  fi
  if [[ "${end}" != *"$pythonappend"* ]]; then
    export PYTHONPATH="${pythonappend}${end}"
    echo "Temporarily using PYTHONPATH as ${PYTHONPATH}"
  fi

  rincpathappend="${PKGDIR}/interface"
  end=""
  if [[ ! -z "${ROOT_INCLUDE_PATH+x}" ]]; then
    end=":${ROOT_INCLUDE_PATH}"
  fi
  if [[ "${end}" != *"$rincpathappend"* ]]; then
    export ROOT_INCLUDE_PATH=${rincpathappend}${end}
    echo "Temporarily using ROOT_INCLUDE_PATH as ${ROOT_INCLUDE_PATH}"
  fi

  pathappend="${PKGDIR}/executables"
  end=""
  if [[ ! -z "${PATH+x}" ]]; then
    end=":${PATH}"
  fi
  if [[ "${end}" != *"$pathappend"* ]]; then
    export PATH="${pathappend}${end}"
    echo "Temporarily using PATH as ${PATH}"
  fi
}
printenvinstr () {
  echo
  echo "Remember to do"
  echo
  echo 'eval $('${BASH_SOURCE[0]}' env)'
  echo "or"
  echo 'eval `'${BASH_SOURCE[0]}' env`'
  echo
  echo "if you are using a bash-related shell, or you can do"
  echo
  echo ${BASH_SOURCE[0]}' env'
  echo
  echo "and change the commands according to your shell in order to do something equivalent to set up the environment variables."
  echo
}

if [[ $doPrintEnv -eq 1 ]]; then
    printenv
    exit
elif [[ $doPrintEnvInstr -eq 1 ]]; then
    printenvinstr
    exit
fi

if [[ $nSetupArgs -eq 0 ]]; then
    setupArgs+=( -j 1 )
    nSetupArgs=2
fi


if [[ "$nSetupArgs" -eq 1 ]] && [[ "${setupArgs[0]}" == *"clean"* ]]; then
    make clean
    exit $?
elif [[ "$nSetupArgs" -ge 1 ]] && [[ "$nSetupArgs" -le 2 ]] && [[ "${setupArgs[0]}" == *"-j"* ]]; then
    : ok
else
    echo "Unknown arguments:"
    echo "  ${setupArgs[@]}"
    echo "Should be nothing, env, envinstr, clean, or -j [Ncores] (and standalone - for standalone compilation)"
    exit 1
fi

doenv

make "${setupArgs[@]}"

compile_status=$?
if [[ ${compile_status} -ne 0 ]]; then
  echo "Compilation failed with status ${compile_status}."
  exit ${compile_status}
fi

printenvinstr

)
