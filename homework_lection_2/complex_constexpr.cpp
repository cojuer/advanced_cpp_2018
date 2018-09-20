class ComplexNumber
{
public:
	constexpr 
	ComplexNumber(int re = 0, int im = 0) :
		m_re(re),
		m_im(im)
	{}

	constexpr
	bool operator==(const ComplexNumber& rhs) const
	{
		return m_re == rhs.m_re && m_im == rhs.m_im;
	}

	constexpr
	void SetRe(int re)
	{
		m_re = re;
	}

	constexpr
	void SetIm(int im)
	{
		m_im = im;
	}

	constexpr
	int GetIm() const
	{
		return m_im;
	}

	constexpr
	int GetRe() const
	{
		return m_re;
	}

private:
	int m_re;
	int m_im;
};

constexpr ComplexNumber Conjugate(const ComplexNumber& x) {
	ComplexNumber res;
	res.SetRe(x.GetRe());
	res.SetIm(-x.GetIm());
	return res;
}
int main() {
	constexpr ComplexNumber a(1, 2);
	constexpr ComplexNumber b(1, -2);
	constexpr auto c = Conjugate(a);
	static_assert(b == c, "failed");
}
