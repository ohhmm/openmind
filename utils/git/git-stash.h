#pragma once

namespace git {

class GitStashScope {
public:
    static void stash();
    static void pop();

    GitStashScope();
    ~GitStashScope();
};

} // namespace git
