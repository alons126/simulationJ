#!/bin/csh

# Re-pulling repository
# ============================================================================
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

# Re-pulling repository
# ============================================================================

source ./all_uni_job_submission_script.csh


echo
echo "\033[35m- FINISHED ------------------------------------------------------------\033[0m"
echo