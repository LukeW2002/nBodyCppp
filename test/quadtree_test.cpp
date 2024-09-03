#include <gtest/gtest.h>
#include "../include/quadtree.hpp"
#include "../include/point.hpp"
#include "../include/constants.hpp"

class QuadTreeTest:public::testing::Test{
	protected:
		void SetUp() override{
			root = std::make_unique<QuadTree>(Node(0,0, 100, 100), 0);
		}
		std::unique_ptr<QuadTree> root;
};

TEST_F(QuadTreeTest, InsertPointInBounds) {
    auto point = std::make_shared<Point>(25, 25, 0, 0, 1, 0, 0);
    EXPECT_TRUE(root->insert(point));
    EXPECT_EQ(root->getPoints().size(), 1);
}

TEST_F(QuadTreeTest, InsertPointOutOfBounds) {
    auto point = std::make_shared<Point>(150, 150, 0, 0, 1, 0, 0);
    EXPECT_FALSE(root->insert(point));
    EXPECT_EQ(root->getPoints().size(), 0);
}

TEST_F(QuadTreeTest, SubdivideTest) {
    for (int i = 0; i < Constants::quadTreeDepth + 1; ++i) {
        auto point = std::make_shared<Point>(25 + i, 25 + i, 0, 0, 1, 0, 0);
        root->insert(point);
    }
    EXPECT_TRUE(root->subdivide());
    EXPECT_EQ(root->getPoints().size(), 0);
}

TEST_F(QuadTreeTest, MassDistributionTest) {
    auto point1 = std::make_shared<Point>(25, 25, 0, 0, 10, 0, 0);
    auto point2 = std::make_shared<Point>(75, 75, 0, 0, 20, 0, 0);
    root->insert(point1);
    root->insert(point2);
    EXPECT_NEAR(root->getTotalMass(), 30, 1e-6);
    EXPECT_NEAR(root->getCentreMassX(), 58.33333333333, 1e-5);
    EXPECT_NEAR(root->getCentreMassY(), 58.33333333333, 1e-5);
}

TEST_F(QuadTreeTest, ForceCalculationTest) {
    auto point1 = std::make_shared<Point>(25, 25, 0, 0, 1e10, 0, 0);
    auto point2 = std::make_shared<Point>(75, 75, 0, 0, 1, 0, 0);
    root->insert(point1);
    root->insert(point2);
    root->calcforce(*point2, 1.0);
    EXPECT_GT(point2->getFx(), 0);
    EXPECT_GT(point2->getFy(), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
