//
// Copyright (c) 2016-2020 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// https://boost-ext.github.io/sml/examples.html
#include "sml.hpp"
#include <cassert>
#include <iostream>
#include <functional>

namespace sml = boost::sml;


class NPC
{
public:
    NPC() {

    }
    ~NPC() {

    }

    bool A() {
        std::cout << "待机" << std::endl;
        return true;
    }

    bool B() {
        std::cout << "巡逻" << std::endl;
        return true;
    }

    bool C() {
        std::cout << "追击" << std::endl;
        return true;
    }

    bool D() {
        std::cout << "攻击" << std::endl;
        return true;
    }

    bool E() {
        std::cout << "逃跑" << std::endl;
        return true;
    }

private:

};


namespace {
    // events
    struct release {
    };
    struct ack {
    };
    struct ackH {
    };
    struct fin {
    };
    struct timeout {
    };


    // actions
    const auto send_fin = []() {
        std::cout << "send_fin OK!" << std::endl;
        };
    const auto send_ack = []() {
        std::cout << "send_ack OK!" << std::endl;
        };

    

#if !defined(_MSC_VER)
    struct hello_world {
        auto operator()() const {
            using namespace sml;
            // clang-format off
            return make_transition_table(
                *"established"_s + event<release> / send_fin = "fin wait 1"_s,
                "fin wait 1"_s + event<ack>[is_ack_valid] = "fin wait 2"_s,
                "fin wait 2"_s + event<fin>[is_fin_valid] / send_ack = "timed wait"_s,
                "timed wait"_s + event<timeout> / send_ack = X
            );
            // clang-format on
        }
    };
}

int main() {
    using namespace sml;

    sm<hello_world> sm;
    static_assert(1 == sizeof(sm), "sizeof(sm) != 1b");
    assert(sm.is("established"_s));

    sm.process_event(release{});
    assert(sm.is("fin wait 1"_s));

    sm.process_event(ack{});
    assert(sm.is("fin wait 2"_s));

    sm.process_event(fin{});
    assert(sm.is("timed wait"_s));

    sm.process_event(timeout{});
    assert(sm.is(X));  // released
}
#else
    class established;
    class fin_wait_1;
    class fin_wait_2;
    class timed_wait;

    struct hello_world {

        NPC* Smnpc = nullptr;

        hello_world(NPC* npc):Smnpc(npc){}

        auto operator()() const {
            using namespace sml;

            // clang-format off
            return make_transition_table(
                *state<established> +event<release>[std::bind(&NPC::A, Smnpc)] / send_fin = state<fin_wait_1>,
                state<fin_wait_1> + sml::on_entry<_> / [] { std::cout << "s1 on entry" << std::endl; },
                state<fin_wait_1> + sml::on_exit<_> / [] { std::cout << "s1 on exit" << std::endl; },
                state<fin_wait_1> +event<ack>[std::bind(&NPC::B, Smnpc)],
                state<fin_wait_1> +event<ackH>[std::bind(&NPC::C, Smnpc)] = state<fin_wait_2>,
                state<fin_wait_2> +event<fin>[std::bind(&NPC::D, Smnpc)] / send_ack = state<timed_wait>,
                state<timed_wait> +event<timeout>[std::bind(&NPC::E, Smnpc)] / send_ack = X
            );
            // clang-format on
        }
    };

    struct hello {
        auto operator()() const {
            std::cout << "999" << std::endl;
            return 666;
        }
    };
}

int main() {
    using namespace sml;
    hello xx;
    std::cout << "operator()() : " << xx() << std::endl;




    NPC* mnpc = new NPC();
    hello_world Mhello_world(mnpc);

    sm<hello_world> sm{ Mhello_world };
    assert(sm.is(state<established>));

    sm.process_event(release{});
    assert(sm.is(state<fin_wait_1>));

    sm.process_event(ack{});
    sm.process_event(ack{});
    sm.process_event(ack{});
    sm.process_event(ackH{});
    assert(sm.is(state<fin_wait_2>));

    sm.process_event(fin{});
    assert(sm.is(state<timed_wait>));
    
    sm.process_event(timeout{});
    assert(sm.is(X));  // released

    delete mnpc;
}
#endif
