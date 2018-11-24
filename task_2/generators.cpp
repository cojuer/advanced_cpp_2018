#include <iostream>
#include <algorithm>
#include <random>
#include <memory>
#include <map>
#include <cassert>

constexpr double eps = 0.0000000001;
constexpr double average_eps = 0.1;
constexpr double num_attempts = 1000000;

bool is_valid_prob(double prob)
{
    return prob > 0 - eps && prob < 1 + eps;
}

struct RNGOptions {
    virtual ~RNGOptions() = default;

    virtual bool valid() const = 0;
};

struct PoissonRNGOpts : public RNGOptions
{
    PoissonRNGOpts(double lambda)
        : m_lambda(lambda)
    {}

    bool valid() const override
    {
        return m_lambda > 0;
    }

    double m_lambda;
};

struct BernoulliRNGOpts : public RNGOptions
{
    BernoulliRNGOpts(double prob)
        : m_prob(prob)
    {}

    bool valid() const override
    {
        return is_valid_prob(m_prob);
    }

    double m_prob;
};

struct GeometricRNGOpts : public RNGOptions
{
    GeometricRNGOpts(double prob)
        : m_prob(prob)
    {}

    bool valid() const override
    {
        return is_valid_prob(m_prob);
    }

    double m_prob;
};

struct FiniteRNGOpts : public RNGOptions
{
    FiniteRNGOpts(std::vector<double> probs, std::vector<double> values)
        : m_values(values), m_probs(probs)
    {}

    bool valid() const override
    {
        auto prob_sum = std::accumulate(m_probs.begin(), m_probs.end(), 0.0);
        return (
            m_values.size() == m_probs.size()
            && std::all_of(m_probs.begin(), m_probs.end(), is_valid_prob)
            && abs(1 - prob_sum) < eps 
        );
    }

    std::vector<double> m_values;
    std::vector<double> m_probs;
};

class BaseRNG
{
public:
    virtual ~BaseRNG() = default;
    virtual double generate() const = 0;
};

class PoissonRNG final : public BaseRNG
{
public:
    using OptType = PoissonRNGOpts;

    PoissonRNG(std::unique_ptr<OptType> opts)
        : m_opts(std::move(opts)), m_generator(0), m_distribution(m_opts->m_lambda)
    {}

    double generate() const override
    {
        return m_distribution(m_generator);
    }

private:
    std::unique_ptr<OptType> m_opts;
    mutable std::poisson_distribution<int> m_distribution;
    mutable std::default_random_engine m_generator;
};

class BernoulliRNG final : public BaseRNG
{
public:
    using OptType = BernoulliRNGOpts;

    BernoulliRNG(std::unique_ptr<OptType> opts) 
        : m_opts(std::move(opts)), m_distribution(m_opts->m_prob)
    {}

    double generate() const override
    {
        return m_distribution(m_generator);
    }

private:
    std::unique_ptr<OptType> m_opts;
    mutable std::bernoulli_distribution m_distribution;
    mutable std::default_random_engine m_generator;
};

class GeometricRNG final : public BaseRNG
{
public:
    using OptType = GeometricRNGOpts;

    GeometricRNG(std::unique_ptr<OptType>&& opts)
        : m_opts(std::move(opts)), m_distribution(m_opts->m_prob)
    {}

    double generate() const override
    {
        return m_distribution(m_generator);
    }

private:
    std::unique_ptr<OptType> m_opts;
    mutable std::default_random_engine m_generator;
    mutable std::geometric_distribution<int> m_distribution;
};

class FiniteRNG final : public BaseRNG
{
public:
    using OptType = FiniteRNGOpts;

    FiniteRNG(std::unique_ptr<OptType>&& opts)
        : m_opts(std::move(opts)), m_distribution(m_opts->m_probs.begin(), m_opts->m_probs.end())
    {}

    double generate() const override
    {
        return m_opts->m_values[m_distribution(m_generator)];
    }

private:
    std::unique_ptr<OptType> m_opts;
    mutable std::default_random_engine m_generator;
    mutable std::discrete_distribution<int> m_distribution;
};

class ICreator {
public:
    virtual ~ICreator(){}
    virtual std::unique_ptr<BaseRNG> create(std::unique_ptr<RNGOptions>&& opts) const = 0;
};

template <class TCurrentObject>
class TCreator : public ICreator{
    std::unique_ptr<BaseRNG> create(std::unique_ptr<RNGOptions>&& opts) const override {
        auto typedOpts = dynamic_cast<typename TCurrentObject::OptType*>(opts.get());
        opts.release();
        if (!typedOpts || !typedOpts->valid()) return nullptr;
        return std::make_unique<TCurrentObject>(std::unique_ptr<typename TCurrentObject::OptType>(typedOpts));
    }
};

class Factory {
public:
    Factory() { 
        regAll(); 
    }
    
    template <typename T>
    void regCreator(std::string name) {
        m_creators[name] = std::make_unique<TCreator<T>>();
    }
    
    void regAll() {
        regCreator<PoissonRNG>("poisson");
        regCreator<BernoulliRNG>("bernoulli");
        regCreator<GeometricRNG>("geometric");
        regCreator<FiniteRNG>("finite");
    }

    std::unique_ptr<BaseRNG> create(const std::string& name, std::unique_ptr<RNGOptions>&& options) const {
        auto creator = m_creators.find(name);
        if (creator == m_creators.end()) {
            return nullptr;
        }
        return creator->second->create(std::move(options));
    }
private:
    std::map<std::string, std::unique_ptr<ICreator>> m_creators;
};

void testRNG(BaseRNG& rng, double expectedAverage)
{
    auto sum = 0.0;
    for (int i = 0; i < num_attempts; ++i)
    {
        sum += rng.generate();
    }
    std::cout << sum / num_attempts << ":" << expectedAverage << std::endl;
    assert(abs(sum / num_attempts - expectedAverage) < average_eps);
}

void testPoisson(Factory& f, double lambda)
{
    auto poi = f.create("poisson", std::make_unique<PoissonRNGOpts>(lambda));
    assert(poi);

    auto sum = 0.0;
    for (int i = 0; i < num_attempts; ++i)
    {
        sum += poi->generate();
    }
    std::cout << sum / num_attempts << ":" << lambda << std::endl;
    assert(abs(sum / num_attempts - lambda) < average_eps);
}

void testBernoulli(Factory& f, double prob)
{
    auto ber = f.create("bernoulli", std::make_unique<BernoulliRNGOpts>(prob));
    assert(ber);

    auto sum = 0.0;
    for (int i = 0; i < num_attempts; ++i)
    {
        sum += ber->generate();
    }
    std::cout << sum / num_attempts << ":" << prob << std::endl;
    assert(abs(sum / num_attempts - prob) < average_eps);
}

void testGeometric(Factory& f, double prob)
{
    auto geo = f.create("geometric", std::make_unique<GeometricRNGOpts>(prob));
    assert(geo);

    auto sum = 0.0;
    for (int i = 0; i < num_attempts; ++i)
    {
        sum += geo->generate();
    }

    auto average = (1 - prob) / prob;
    std::cout << sum / num_attempts << ":" << average << std::endl;
    assert(abs(sum / num_attempts - average) < average_eps);
}

void testFinite(Factory& f, std::vector<double> probs, std::vector<double> values)
{
    auto fin = f.create("finite", std::make_unique<FiniteRNGOpts>(probs, values));
    assert(fin);

    auto sum = 0.0;
    for (int i = 0; i < num_attempts; ++i)
    {
        sum += fin->generate();
    }

    auto average = 0.0;
    for (size_t i = 0; i < probs.size(); ++i)
    {
        average += probs[i] * values[i];
    }
    std::cout << sum / num_attempts << ":" << average << std::endl;
    assert(abs(sum / num_attempts - average) < average_eps);
}

int main(void)
{
    auto f = Factory();

    // Test poisson
    testPoisson(f, 0.5);
    testPoisson(f, 0.7);
    testPoisson(f, 0.1);
    
    // Test bernoulli
    testBernoulli(f, 0.5);
    testBernoulli(f, 0.7);
    testBernoulli(f, 0.1);
    
    // Test geometric
    testGeometric(f, 0.5);
    testGeometric(f, 0.7);
    testGeometric(f, 0.1);

    // Test finite
    testFinite(
        f,
        {0.5, 0.5}, 
        {0, 1}
    );

    testFinite(
        f,
        {0.2, 0.3, 0.3, 0.2}, 
        {0, 1, 2, 3}
    );

    testFinite(
        f,
        {0.1, 0.9}, 
        {0, 100}
    );

    // Test invalid
    auto poiInvalid1 = f.create("poisson", std::make_unique<BernoulliRNGOpts>(0.5));
    assert(!poiInvalid1);

    auto poiInvalid2 = f.create("poisson", std::make_unique<PoissonRNGOpts>(-0.5));
    assert(!poiInvalid2);

    return 0;
}