import unittest

import cpp_essentials


class TestUtilsBindings(unittest.TestCase):
    def test_RandomGenerator(self):
        """Ensure that the random generator can be created and used."""
        rng = cpp_essentials.RandomGenerator()
        self.assertIsNotNone(rng)

        # Generate a random number
        random_number = rng()
        self.assertTrue(isinstance(random_number, int))

        rng = cpp_essentials.RandomGenerator(seed=42)
        self.assertIsNotNone(rng)
        r1 = rng()

        rng2 = cpp_essentials.RandomGenerator(seed=42)
        self.assertIsNotNone(rng2)
        r2 = rng2()
        self.assertEqual(r1, r2)


if __name__ == "__main__":
    unittest.main()
