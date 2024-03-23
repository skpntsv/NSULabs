#!/bin/bash

REPO_PATH="PATH_TO_REP"
N_DAYS=30
YOUR_EMAIL="YOUR_EMAIL"


cd "$REPO_PATH" || exit
REPO_NAME=$(basename "$(git rev-parse --show-toplevel)")

BRANCHES=$(git branch -r | grep -v 'origin/HEAD' | sed 's/origin\///')

for BRANCH in $BRANCHES; do

    LAST_COMMIT_INFO=$(git log --format="%at %an %ae %ai" -n 1 "origin/$BRANCH")
    LAST_COMMIT_TIMESTAMP=$(echo "$LAST_COMMIT_INFO" | awk '{print $1}')
    AUTHOR_NAME=$(echo "$LAST_COMMIT_INFO" | awk '{print $2}')
    AUTHOR_EMAIL=$(echo "$LAST_COMMIT_INFO" | awk '{print $3}')
    LAST_COMMIT_DATE=$(echo "$LAST_COMMIT_INFO" | awk '{print $4}')

    CURRENT_TIMESTAMP=$(date +%s)

    DAYS_SINCE_LAST_COMMIT=$(( (CURRENT_TIMESTAMP - LAST_COMMIT_TIMESTAMP) / (60*60*24) ))


    if [ "$DAYS_SINCE_LAST_COMMIT" -gt "$N_DAYS" ]; then
        echo "Branch: $BRANCH"
        echo "Last commit date: $LAST_COMMIT_DATE"
        echo "Author: $AUTHOR_NAME"
        echo "Author's email: $AUTHOR_EMAIL"
        echo "Days since last commit: $DAYS_SINCE_LAST_COMMIT"
        echo "+++++++++++++++++++++"

        # Send email to author
        if [[ "$AUTHOR_EMAIL" == *.*@*.* ]]; then
            subject="[$REPO_NAME] - Your branch $BRANCH needs attention"
            body="Hello $AUTHOR_NAME,\n\nYour branch $BRANCH in the repository $REPO_NAME hasn't been updated for $DAYS_SINCE_LAST_COMMIT days. Please review and update it as necessary.\n\nBest regards,\nYour Git Bot"
            #echo "Subject: $subject"
            #echo "Body: $body"
            echo -e "$body" | mailx -s "$subject" -aFrom:"Git Bot <$YOUR_EMAIL>" "$AUTHOR_EMAIL"
            echo "Email sent to $AUTHOR_EMAIL"
        else
            echo "Invalid email address: $AUTHOR_EMAIL. Skipping..."
        fi
        echo "---------------------"
    fi
done