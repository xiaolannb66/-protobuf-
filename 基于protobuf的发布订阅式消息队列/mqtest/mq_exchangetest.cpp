#include "../mqserver/mq_exchange.hpp"
#include <gtest/gtest.h>

bitmq::ExchangeManager::ptr emp;

class ExchangeTest : public testing::Environment {
    public:
        virtual void SetUp() override {
            emp = std::make_shared<bitmq::ExchangeManager>("./data/meta.db");
        }
        virtual void TearDown() override {
            //emp->clear();
            std::cout << "最后的清理！！\n";
        }
};

TEST(exchange_test, insert_test) {
    std::unordered_map<std::string, std::string> map;
    emp->declareExchange("exchange1", bitmq::ExchangeType::DIRECT, true, false, map);
    emp->declareExchange("exchange2", bitmq::ExchangeType::DIRECT, true, false, map);
    emp->declareExchange("exchange3", bitmq::ExchangeType::DIRECT, true, false, map);
    emp->declareExchange("exchange4", bitmq::ExchangeType::DIRECT, true, false, map);
    ASSERT_EQ(emp->size(), 4);
}
TEST(exchange_test, select_test) {
    ASSERT_EQ(emp->exists("exchange1"), true);
    ASSERT_EQ(emp->exists("exchange2"), true);
    ASSERT_EQ(emp->exists("exchange3"), false);
    ASSERT_EQ(emp->exists("exchange4"), true);
    bitmq::Exchange::ptr exp = emp->selectExchange("exchange2");
    ASSERT_NE(exp.get(), nullptr);
    ASSERT_EQ(exp->name, "exchange2");
    ASSERT_EQ(exp->durable, true);
    ASSERT_EQ(exp->auto_delete, false);
    ASSERT_EQ(exp->type, bitmq::ExchangeType::DIRECT);
    ASSERT_EQ(exp->getArgs(), std::string("k1=v1&k2=v2&"));
}

TEST(exchange_test, remove_test) {
    emp->deleteExchange("exchange2");
    bitmq::Exchange::ptr exp = emp->selectExchange("exchange2");
    ASSERT_EQ(exp.get(), nullptr);
    ASSERT_EQ(emp->exists("exchange2"), false);
}

int main(int argc,char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(new ExchangeTest);
    RUN_ALL_TESTS();
    return 0;
}