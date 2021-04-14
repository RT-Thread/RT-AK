Feature: user1story1
    https://git.rt-thread.com/research/edge-ai/-/issues/246

    Scenario Outline: run rt_ai_tools
        Given there is a <model> base on <platform>
        When I run aitools
        Then I would get the <dir> in project

        Examples: Vertical
        | model   | ../rt_ai_tools/Models/keras_mnist.h5 |
        | platform | stm32 |
        | dir | rt_ai_lib |


    Scenario Outline: load rt_ai_lib test to project
        Given there is a project
        When add rt_ai_lib to project
        And I complie the project
        Then I would get a SConstruct file include "RTAK_TEST"
        And I would get a <firmware>

        Examples: Vertical
        | firmware  | rtthread.bin |