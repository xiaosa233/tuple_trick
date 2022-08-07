class tuple_trick:
    def __init__(self, config):
        self.output = config.Output.STATIC_LIB
        self.third_parties = [
            "googletest",
        ]
