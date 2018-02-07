#!/bin/bash
echo "==> Init gitflow"
git flow init -d
echo "==> Configure gitflow"
git config gitflow.feature.start.fetch true
git config gitflow.feature.finish.no-ff true
git config gitflow.feature.finish.keep true
git config gitflow.feature.finish.keepremote true
git config gitflow.feature.finish.keeplocal true
git config gitflow.feature.delete.remote true

echo "==> Configure git"
git config push.default simple

TEMPLATE_PATH="$(git rev-parse --show-toplevel)/.git/COMMIT_TEMPLATE"

cat > $TEMPLATE_PATH << EOF
# [<task>] <type>: (If applied, this commit will...) <subject> (Max 50 char)
# |<----  Using a Maximum Of 50 Characters  ---->|


# Explain why this change is being made
# |<----   Try To Limit Each Line to a Maximum Of 72 Characters   ---->|

# Provide links or keys to any relevant tickets, articles or other resources
# Example: Github issue #23

# --- COMMIT END ---
# Type can be
#    feat     (new feature)
#    fix      (bug fix)
#    refactor (refactoring production code)
#    style    (formatting, missing semi colons, etc; no code change)
#    docs     (changes to documentation)
#    test     (adding or refactoring tests; no production code change)
#    chore    (updating grunt tasks etc; no production code change)
# --------------------
# Remember to
#    Capitalize the subject line
#    Use the imperative mood in the subject line
#    Do not end the subject line with a period
#    Separate subject from body with a blank line
#    Use the body to explain what and why vs. how
#    Can use multiple lines with "-" for bullet points in body
# --------------------
# For more information about this template, check out
# https://gist.github.com/adeekshith/cd4c95a064977cdc6c50
EOF
git config commit.template $TEMPLATE_PATH

echo "==> Setup gitflow-avh bash-completion"
COMP_PATH=/etc/bash_completion.d
if [[ -d $COMP_PATH && ! -f $COMP_PATH/git_flow_avh.completion.bash ]]; then
    read -p "Install gitflow-avh bash-completion (y/N)? " answer
    case ${answer:0:1} in
        y|Y )
            echo "  -> Installing"
            sudo wget https://raw.githubusercontent.com/Bash-it/bash-it/master/completion/available/git_flow_avh.completion.bash -O $COMP_PATH/git_flow_avh.completion.bash
            echo "Restart terminal for completion to start working"
        ;;
        * )
            echo "Not installing gitflow-avh bash-completion"
        ;;
    esac
fi;
echo "==> Done"
