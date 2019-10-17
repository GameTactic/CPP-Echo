workflow "Deployment" {
  on = "release"
  resolves = ["maddox/actions/ssh@master"]
}

action "Sleep" {
  uses = "maddox/actions/sleep@master"
  args = "3600"
}

action "maddox/actions/ssh@master" {
  uses = "maddox/actions/ssh@master"
  needs = ["Sleep"]
  secrets = ["USER", "HOST", "PRIVATE_KEY"]
  args = "whoami"
}
