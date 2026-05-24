#include "Account.h"
#include "Transaction.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class AccountMock : public Account {
public:
	AccountMock(int id, int balance) : Account(id, balance) {}
	MOCK_CONST_METHOD0(GetBalance, int());
	MOCK_METHOD1(ChangeBalance, void(int diff));
	MOCK_METHOD0(Lock, void());
	MOCK_METHOD0(Unlock, void());
};

class TransactionMock : public Transaction {
public:
	MOCK_METHOD3(Make, bool(Account& from, Account& to, int sum));
};

TEST(Account, SimpleTest) {
	Account acc(1, 100);
	EXPECT_EQ(acc.id(), 1);
	EXPECT_EQ(acc.GetBalance(), 100);
	EXPECT_THROW(acc.ChangeBalance(100), std::runtime_error);
	EXPECT_NO_THROW(acc.Lock());
	acc.ChangeBalance(100);
	EXPECT_EQ(acc.GetBalance(), 200);
	EXPECT_THROW(acc.Lock(), std::runtime_error);
}

TEST(Transaction, SimpleTest) {
	Transaction tr;
	Account ac1(1, 50);
	Account ac2(2, 500);
	tr.set_fee(100);
	EXPECT_EQ(tr.fee(), 100);
	EXPECT_THROW(tr.Make(ac1, ac1, 0), std::logic_error);
	EXPECT_THROW(tr.Make(ac1, ac2, -1), std::invalid_argument);
	EXPECT_THROW(tr.Make(ac1, ac2, 99), std::logic_error);
	EXPECT_FALSE(tr.Make(ac1, ac2, 199));
	EXPECT_FALSE(tr.Make(ac2, ac1, 500));
	EXPECT_TRUE(tr.Make(ac2, ac1, 300));
}

TEST(Transaction, Mock) {
	using ::testing::InSequence;
	using ::testing::Return;

	Transaction tr;
	tr.set_fee(100);

	AccountMock ac1(1, 50);
	AccountMock ac2(2, 500);

	{
		InSequence seq;

		// ошибка: комиссия*2 > сумма
		EXPECT_FALSE(tr.Make(ac1, ac2, 199));

		// ошибка: 500 < 600
		// Guard
		EXPECT_CALL(ac2, Lock());
		EXPECT_CALL(ac1, Lock());
		EXPECT_CALL(ac1, ChangeBalance(500));
		EXPECT_CALL(ac2, GetBalance()).WillOnce(Return(500));
		EXPECT_CALL(ac1, ChangeBalance(-500)); // откат
		// Сохранение в базу данных
		EXPECT_CALL(ac2, GetBalance()).WillOnce(Return(500));
		EXPECT_CALL(ac1, GetBalance()).WillOnce(Return(50));
		// деструктор Guard
		EXPECT_CALL(ac1, Unlock());
		EXPECT_CALL(ac2, Unlock());
		EXPECT_FALSE(tr.Make(ac2, ac1, 500));

		// 500 >= 400
		EXPECT_CALL(ac2, Lock());
		EXPECT_CALL(ac1, Lock());
		EXPECT_CALL(ac1, ChangeBalance(300));
		EXPECT_CALL(ac2, GetBalance()).WillOnce(Return(500));
		EXPECT_CALL(ac2, ChangeBalance(-400));
		// Сохранение в базу данных
		EXPECT_CALL(ac2, GetBalance()).WillOnce(Return(100)); // 500-400
		EXPECT_CALL(ac1, GetBalance()).WillOnce(Return(350)); // 50+300
		EXPECT_CALL(ac1, Unlock());
		EXPECT_CALL(ac2, Unlock());
		EXPECT_TRUE(tr.Make(ac2, ac1, 300));

		// проверка ошибок
		EXPECT_THROW(tr.Make(ac1, ac1, 0), std::logic_error);
		EXPECT_THROW(tr.Make(ac1, ac2, -1), std::invalid_argument);
		EXPECT_THROW(tr.Make(ac1, ac2, 99), std::logic_error);
	}
}
