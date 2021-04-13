Feature: U1S1-3
	https://git.rt-thread.com/research/edge-ai-group/edge-ai/-/issues/247

    Scenario Outline: Whether RT-AK docs have given the models supported lists
        Given I got one file <file>
        When I read the file
        Then the results should have some platform support

        Examples:
        | file          |
        | ../../README.md  |

    @error
    Scenario: not file exists
        Given I got one file <file>
        Then the file could not be read

        Examples:
        | file          |
        | ../../READ.md    |

    
    @error
    Scenario: too many platforms
        Given I got one file <file>
        When I read the file
        Then There are too many <platforms>

        Examples:
        | file          |  platforms                  | 
        | ../../README.md  |  "stm32 k210 tflite"        |