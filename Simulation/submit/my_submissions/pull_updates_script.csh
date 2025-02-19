#!/bin/csh

echo "\033[35m- Running update and submission script -------------------------------\033[0m"
echo

unset MY_SUBMISSION_DIR
setenv MY_SUBMISSION_DIR `pwd`
echo "\033[35mMY_SUBMISSION_DIR:\033[0m ${MY_SUBMISSION_DIR}"

# Re-pulling repository
# ============================================================================
echo
echo "\033[35m- Re-pulling repository -----------------------------------------------\033[0m"
echo
echo "\033[35mResetting git...\033[0m"
git reset --hard
echo
echo "\033[35mCleaning excessive files...\033[0m"
git clean -f
echo
echo "\033[35mPulling updates...\033[0m"
git pull
echo

# Sourcing submission scripts
# ============================================================================
echo "\033[35m- Sourcing submission scripts -----------------------------------------\033[0m"
echo
echo
source submission_script.csh
echo

echo
cd $SUBMISSION_DIR
echo "\033[35m- FINISHED ------------------------------------------------------------\033[0m"
echo