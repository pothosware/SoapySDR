// Copyright Matthew King 2012-2013.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

using System.Collections.Generic;

/// <summary>
/// Provides functionality assist with the generation of hash codes.
/// </summary>
internal sealed class HashCodeBuilder
{
    /// <summary>
    /// Multiplier for the argument hashes.
    /// </summary>
    private readonly int multiplier;

    /// <summary>
    /// Current hash value.
    /// </summary>
    private int hashCode;

    /// <summary>
    /// Initializes a new instance of the HashCodeBuilder class.
    /// </summary>
    /// <param name="seed">Initial value for the hash.</param>
    /// <param name="multiplier">Multiplier for the argument hashes.</param>
    private HashCodeBuilder(int seed, int multiplier)
    {
        this.multiplier = multiplier;
        this.hashCode = seed;
    }

    /// <summary>
    /// Includes the specified value in the hash code.
    /// </summary>
    /// <typeparam name="T">Type of the value to include in the hash code.</typeparam>
    /// <param name="value">Value to include in the hash code.</param>
    /// <returns>This HashCodeBuilder instance.</returns>
    public HashCodeBuilder AddValue<T>(T value)
    {
        return this.AddValue<T>(value, null);
    }

    /// <summary>
    /// Includes the specified value in the hash code.
    /// </summary>
    /// <typeparam name="T">Type of the value to include in the hash code.</typeparam>
    /// <param name="value">Value to include in the hash code.</param>
    /// <param name="comparer">IEqualityComparer&lt;T&gt; to calculate the hash code.</param>
    /// <returns>This HashCodeBuilder instance.</returns>
    public HashCodeBuilder AddValue<T>(T value, IEqualityComparer<T> comparer)
    {
        if (comparer == null)
        {
            comparer = EqualityComparer<T>.Default;
        }

        unchecked
        {
            int hc = value != null ? comparer.GetHashCode(value) : 0;
            this.hashCode = (this.hashCode * this.multiplier) + hc;
        }

        return this;
    }

    /// <summary>
    /// Includes the specified values in the hash code.
    /// </summary>
    /// <typeparam name="T">Type of the values to include in the hash code.</typeparam>
    /// <param name="values">
    /// An enumerator that iterates through the sequence of values to include in the hash code.
    /// </param>
    /// <returns>This HashCodeBuilder instance.</returns>
    public HashCodeBuilder AddValues<T>(IEnumerable<T> values)
    {
        return this.AddValues<T>(values, null);
    }

    /// <summary>
    /// Includes the specified values in the hash code.
    /// </summary>
    /// <typeparam name="T">Type of the values to include in the hash code.</typeparam>
    /// <param name="values">
    /// An enumerator that iterates through the sequence of values to include in the hash code.
    /// </param>
    /// <param name="comparer">
    /// IEqualityComparer&lt;T&gt; to calculate the hash code.
    /// </param>
    /// <returns>This HashCodeBuilder instance.</returns>
    public HashCodeBuilder AddValues<T>(IEnumerable<T> values, IEqualityComparer<T> comparer)
    {
        if (comparer == null)
        {
            comparer = EqualityComparer<T>.Default;
        }

        if (values != null)
        {
            foreach (T value in values)
            {
                unchecked
                {
                    int hc = value != null ? value.GetHashCode() : 0;
                    this.hashCode = (this.hashCode * this.multiplier) + hc;
                }
            }
        }

        return this;
    }

    /// <summary>
    /// Returns the current hash code of this HashCodeBuilder instance.
    /// </summary>
    /// <returns>The current hash code.</returns>
    public int ToHashCode()
    {
        return this.hashCode;
    }

    /// <summary>
    /// Implicit conversion from HashCodeBuilder to int.
    /// </summary>
    /// <param name="builder">The HashCodeBuilder instance to convert.</param>
    /// <returns>The current hash code of the HashCodeBuilder instance.</returns>
    public static implicit operator int(HashCodeBuilder builder)
    {
        return builder.ToHashCode();
    }

    /// <summary>
    /// Creates a new HashCodeBuilder instance.
    /// </summary>
    /// <returns>A new HashCodeBuilder instance.</returns>
    public static HashCodeBuilder Create()
    {
        return new HashCodeBuilder(17, 29);
    }
}