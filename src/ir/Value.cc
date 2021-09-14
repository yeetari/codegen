#include <ir/Value.hh>

#include <support/Assert.hh>

#include <algorithm>

namespace ir {

Value::~Value() {
    replace_all_uses_with(nullptr);
}

void Value::add_user(Value *user) {
    ASSERT(!m_users.contains(user));
    m_users.insert(user);
}

void Value::remove_user(Value *user) {
    ASSERT(m_users.contains(user));
    m_users.erase(user);
}

void Value::replace_all_uses_with(Value *repl) {
    for (auto *user : std::unordered_set(m_users)) {
        user->replace_uses_of_with(this, repl);
    }
    ASSERT(m_users.empty());
}

void Value::replace_uses_of_with(Value *, Value *) {
    ENSURE_NOT_REACHED();
}

} // namespace ir
